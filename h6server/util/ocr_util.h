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

	Point getLeftEndpointOfLine(Vec2f line); /*  直线与图片左边缘的交点*/
	Point getRightEndpointOfLine(Vec2f line, float width);

	bool compareLine(Vec2f a, Vec2f b);  //比较直线截距大小
	bool compareLine_theta(Vec2f a, Vec2f b);  //比较直线斜率大小

	void _filterLines(vector<Vec2f>& lines, float lo, float hi);
	void findLines_Horizonta(vector<Vec2f>& lines); // 筛选出横线,斜率范围： [M_PI / 2 - 0.05, M_PI / 2 + 0.05)
	void findLines_Vertical(vector<Vec2f>& lines);// 筛选出竖线,斜率范围： [-0.01, 0.01)

	void findHorizontaLinesNearTarget(vector<Vec2f>& lines, float target);/* 所有横线中找到纵坐标跟值target最接近的两条直线*/
	void findHorizontaLines_MaxInterval(vector<Vec2f>& lines);/* 找到间距最大的两条横线*/

	void drawLines(Mat &image, vector<Vec2f> &lines);  /* 在image上画直线 */
	void drawRectangles(Mat &image, vector<Rect>& rects);  /* 在image上画矩形 */

	void _calCutPosition_Horizontal(const Mat& canny_image, vector<float>& position, int count_threshold);
	void _cut_Horizontal(const Mat& canny_image, Rect rect, vector<Rect>& rects, int threshold);
	int cut_Horizontal(const Mat& canny_image, Rect rect, vector<Rect>& rects);  /* 横向切割得到多个矩形， rects保存结果 */
	void _calcutPosition_Vertical(const Mat& canny_image, vector<float>& position, int count_threshold);
	int cut_Vertical(const Mat& canny_image, Rect rect, vector<Rect>& rects);


	Rect cutByRange(Rect rect, RangeLR range); /* 将rect 按照range的范围切割*/

	void batchOCR(tesseract::TessBaseAPI& tess_ocr, const Mat& image, const vector<Rect>& areas, vector<string> &result);

	RangeLR findEndPointsOfLine(Mat image, int idx);

	int detectLines(const Mat& src_image, vector<Vec2f>& lines);  /* 找到所有直线 */
}

