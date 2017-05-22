#include "blood_ocr.h"
#include <time.h> 
#include "../err.h"
#include "../util/my_log.h"
#include "../util/util.h"
#include "../util/common.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

Blood_OCR::Blood_OCR()
{
	initOcrEngine();
}

Blood_OCR::~Blood_OCR()
{
}

int Blood_OCR::initOcrEngine()
{
	_INFO("��ʼ��ocr����");
	tess_ocr_key_.Init(NULL, "fontyp", tesseract::OEM_TESSERACT_ONLY);
	tess_ocr_key_.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
	tess_ocr_value_.Init(NULL, "num", tesseract::OEM_TESSERACT_ONLY);
	tess_ocr_value_.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
	return 0;
}


int Blood_OCR::loadDictionary(string dictionary_string)
{
	_INFO("����Ѫ�����ֵ�");
	Json::Reader reaeder;
	if (!reaeder.parse(dictionary_string, result_))
	{
		_ERROR(dictionary_string);
		return H6OCRERROR::ERR_loadDictionary_Json_parse;
	}

	Json::Value scale = result_["scale"];
	for (Json::Value obj : scale)
	{
		dictionary_.insert(pair<string,string>(obj["text"].asString(), ""));
	}
	return 0;
}


/* �ҵ����� ��  ��λ�ĸ��� �� ͸�ӱ任 */
int Blood_OCR::perspectiveTransformation(const Mat& src_image, Mat& dst_image)
{
	Size src_image_size = src_image.size();

	/* �Һ���*/
	vector<Vec2f> lines;
	{
		if (detectLines(src_image, lines) != 0)
		{
			_ERROR("ֱ������" + to_string(lines.size()));
			return H6OCRERROR::perspective_detectLines;
		}
		//{
		//	Mat tmp_image(src_image);
		//	drawLines(tmp_image, lines);
		//	imshow("����ʶ����", tmp_image);
		//}
		/* �ҵ���Ҫ��ֱ��*/
		findHorizontaLines_MaxInterval(lines);  //����ٶ��������������ֱ��֮������ݾ��Ǵ�ʶ������
	}

	{	/* ��λ�ĸ��� */
		vector<Point2f> corners = {
			getLeftEndpointOfLine(lines.front()),
			getRightEndpointOfLine(lines.front(), src_image_size.width),
			getLeftEndpointOfLine(lines.back()),
			getRightEndpointOfLine(lines.back(), src_image_size.width)
		};

		Size2f dst_size = { (float)src_image_size.width, lines.back()[0] - lines.front()[0] };

		vector<Point2f> corners_trans = {
			{ 0, 0 }, { dst_size.width, 0 }, { 0, dst_size.height }, { dst_size.width, dst_size.height }
		};
		Mat warp_mat = cv::getPerspectiveTransform(corners, corners_trans);
		cv::warpPerspective(src_image, dst_image, warp_mat, dst_size);  // ͸�ӱ任
	}
	return 0;
}

/* �ҵ��������ֱ�Ե */
int Blood_OCR::findLeftAndRightEdge(const Mat& src_image, Mat& dst_image)
{
	vector<Vec2f> lines;
	{
		Mat canny_image;
		Canny(src_image, canny_image, 50, 200, 3);
		int threshold = 5;  // ��ֵ:5  
		cv::HoughLines(canny_image, lines, 1, CV_PI / 180, threshold, 0, 0);
		findLines_Vertical(lines);
	}

	if (lines.empty())
	{
		dst_image = src_image;
	}
	else
	{
		Vec2f left = lines.front(), right = lines.back();
		Rect rect((int)left[0], 0, right[0] - left[0] + 2, src_image.size().height);
		dst_image = src_image(rect);
	}
	return 0;
}

// ���ֵ䳢��ƥ�䣬����ƥ��ɹ�������
int Blood_OCR::correctKey(vector<string> &keys)
{
	int valid_count = 0;
	for (string& key : keys)
	{
		if (dictionary_.count(key) == 1)  //ֱ���ҵ�
		{
			++valid_count;
			continue;
		}
		/* ���ݱ༭������Сԭ�򣬾���*/
		int minDis = 100;
		string target;
		for (auto name_pair : dictionary_)
		{
			string name = name_pair.first;
			int dis = COMMON::minEditDistance(name, key);
			if (dis < minDis)
			{
				minDis = dis;
				target = name;  //
			}
		}
		if (minDis <= key.size() / 2)
		{
			++valid_count;
			key = target;  //correct
		}
	}
	return valid_count;
}

/* �����Ƿ���float�ж�*/
int Blood_OCR::correctValue(vector<string> &values)
{
	int valid_count = 0;
	for (string& val : values)
	{
		val = COMMON::stripAllSpace(val);
		if (COMMON::isFloat(val))
		{
			++valid_count;
		}
	}
	return valid_count;
}

int Blood_OCR::cutAndOcr(const Mat& image)
{
	Size image_size = image.size();

	Mat canny_image;
	Canny(image, canny_image, 50, 200, 3);  /* ��Ե��� -> ��ɺڰ�ͼ��[���ڼ���] */

	_INFO("�����и�...");
	/* �����и�*/
	vector<Rect> rects;
	{
		Rect image_rect = { 0, 0, image_size.width, image_size.height };
		cut_Vertical(canny_image, image_rect, rects);
		if (rects.empty())
		{
			_ERROR("�����и�Ľ��Ϊ��" );
			return H6OCRERROR::cutAndOcr_cut_Vertical;
		}
		//Mat tmp;
		//cv::cvtColor(image, tmp, CV_GRAY2BGR);
		//drawRectangles(tmp, rects);
		//imshow("�����и�����ʶ��", tmp);
	}

		
	vector<string> keys, values;

	tess_ocr_key_.SetImage(image.data, image.cols, image.rows, 1, image.step);
	tess_ocr_value_.SetImage(image.data, image.cols, image.rows, 1, image.step);

	int key_count_flag = 0; // ��û�ҵ���Ӧvalue�е�key������
	int tmp_idx = 0;
	vector<string> tmp_keys;
	_INFO("�����и�...");
	for (Rect rect : rects)
	{
		++tmp_idx;
		/* �����и �õ����յ�ʶ��С���� */
		vector<Rect> areas;
		if (cut_Horizontal(canny_image, rect, areas) != 0)
		{
			continue; //
		}
		/* debug*/
		//{
		//	Mat tmp_image;
		//	cv::cvtColor(image, tmp_image, CV_GRAY2BGR);
		//	drawRectangles(tmp_image, areas);
		//	//imshow("ʶ������" + std::to_string(tmp_idx), tmp_image);
		//}


		/*
		����ocr --> �ж��Ƿ�����Ҫ������
		key �� value �����, ����һһ��Ӧ���ж���key�о��ж���value��
		*/
		vector<string> result;
		int valid_count = 0;
		if (key_count_flag == 0)
		{
			batchOCR(tess_ocr_key_, image, areas, result);
			valid_count = correctKey(result);
			if (valid_count > result.size() / 3)  // ƥ��ȳ���1/3 -> ����Ϊ��key
			{
				++key_count_flag;
				tmp_keys.assign(result.begin(), result.end());
			}
		}
		else
		{
			batchOCR(tess_ocr_value_, image, areas, result);
			valid_count = correctValue(result);
			if (valid_count > result.size() / 2)  // ƥ��ȳ���һ�� -> ����Ϊ��value
			{
				--key_count_flag;
				/* ȷ��key value ����ͬ*/
				keys.insert(keys.end(), tmp_keys.begin(), tmp_keys.end());
				values.insert(values.end(), result.begin(), result.end());
			}
		}

	}
	if (keys.empty())
	{
		_ERROR("keys ���Ϊ��"); 
		return H6OCRERROR::cutAndOcr_keys_null;
	}
	if (values.empty())
	{
		_ERROR("values ���Ϊ��");
		return H6OCRERROR::cutAndOcr_values_null;
	}
	if ( keys.size() > values.size())
	{
		_ERROR("keys �� values �����Ŀ����");
		return H6OCRERROR::cutAndOcr_keys_greater;;
	}
	if (keys.size() < values.size())
	{
		_ERROR("values �� keys �����Ŀ����");
		return H6OCRERROR::cutAndOcr_values_greater;;
	}

	for (int i = 0; i < keys.size(); ++i)
	{
		string key = keys[i];
		if (dictionary_.count(key) == 1)
		{
			dictionary_[key] = values[i];
		}
	}
	return 0;
}


/*
��ʶ�����Ҫ�� �������������ߣ� Ȼ���зָ���key��value����һһ��Ӧ
����ʶ��˼·��
	ʶ����ߣ�ȡ���߼������������Ϊʶ������
	�����������ߵ��ĸ��ǣ�Ͷ��任�õ������ľ�������
	�����и
	���ÿһ�У�����ʶ�����ʶ���ʴﵽһ����ֵȷ����ʵkey����value��
*/
int Blood_OCR::recognise(const vector<unsigned char>& image_buffer)
{
	_INFO("recognise...");
	Mat src_image = imdecode(Mat(image_buffer), 0);
	if (src_image.data == NULL)
	{
		return INVILD_IMAGE;
	}
	//imshow("��ԭͼ��", src_image);

	_INFO("͸��任...");
	int ret;
	Mat transform_image;
	ret = perspectiveTransformation(src_image, transform_image);
	if (ret != 0) return ret;
	//imshow("��͸��任�����", transform_image);

	_INFO("��λ���ұ߽�...");
	Mat dst_image;
	ret = findLeftAndRightEdge(transform_image, dst_image);
	if (ret != 0) return ret;
	//imshow("����λ���ұ߽�����", dst_image);

	_INFO("�и��ʶ��r...");
	ret = cutAndOcr(dst_image);

	return ret;
}

void Blood_OCR::retrieve(Json::Value& result)
{
	_INFO("ȡ���...");
	Json::Value& scale = result_["scale"];
	for (auto& obj : scale)
	{
		string text = obj["text"].asString();
		if (dictionary_.count(text) == 1)
		{
			obj["value"] = dictionary_[text];
		}
	}
	result["scaleData"] = result_;
}
