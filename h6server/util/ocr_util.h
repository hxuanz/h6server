#pragma once
#include "opencv2/core.hpp"
#include "tesseract/baseapi.h"


namespace OCR_UTIL
{
	using namespace std;
	using namespace cv;
	struct RangeLR
	{
		int left;
		int right;
	};

	struct RangeTB
	{
		int top;
		int bottom;
	};

	Point getLeftEndpointOfLine(Vec2f line); /*  ֱ����ͼƬ���Ե�Ľ���*/
	Point getRightEndpointOfLine(Vec2f line, float width);

	bool compareLine(Vec2f a, Vec2f b);  //�Ƚ�ֱ�߽ؾ��С
	bool compareLine_theta(Vec2f a, Vec2f b);  //�Ƚ�ֱ��б�ʴ�С

	void _filterLines(vector<Vec2f>& lines, float lo, float hi);
	void findLines_Horizonta(vector<Vec2f>& lines); // ɸѡ������,б�ʷ�Χ�� [M_PI / 2 - 0.05, M_PI / 2 + 0.05)
	void findLines_Vertical(vector<Vec2f>& lines);// ɸѡ������,б�ʷ�Χ�� [-0.01, 0.01)

	void findHorizontaLinesNearTarget(vector<Vec2f>& lines, float target);/* ���к������ҵ��������ֵtarget��ӽ�������ֱ��*/
	void findHorizontaLines_MaxInterval(vector<Vec2f>& lines);/* �ҵ����������������*/

	void drawLines(Mat &image, vector<Vec2f> &lines);  /* ��image�ϻ�ֱ�� */
	void drawRectangles(Mat &image, vector<Rect>& rects);  /* ��image�ϻ����� */

	void _calCutPosition_Horizontal(const Mat& canny_image, vector<float>& position, int count_threshold);
	void _cut_Horizontal(const Mat& canny_image, Rect rect, vector<Rect>& rects, int threshold);
	int cut_Horizontal(const Mat& canny_image, Rect rect, vector<Rect>& rects);  /* �����и�õ�������Σ� rects������ */
	void _calcutPosition_Vertical(const Mat& canny_image, vector<float>& position, int count_threshold);
	int cut_Vertical(const Mat& canny_image, Rect rect, vector<Rect>& rects);


	Rect cutByRange(Rect rect, RangeLR range); /* ��rect ����range�ķ�Χ�и�*/

	void batchOCR(tesseract::TessBaseAPI& tess_ocr, const Mat& image, const vector<Rect>& areas, vector<string> &result);

	RangeLR findEndPointsOfLine(Mat image, int idx);

	int detectLines(const Mat& src_image, vector<Vec2f>& lines);  /* �ҵ�����ֱ�� */
}

