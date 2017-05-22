#pragma once
#include "opencv2/core.hpp"
#include "tesseract/baseapi.h"
using namespace std;
using namespace cv;

Point getLeftEndpointOfLine(Vec2f line);
Point getRightEndpointOfLine(Vec2f line, float width);

bool compareLine(Vec2f a, Vec2f b);
bool compareLine_theta(Vec2f a, Vec2f b);

void _filterLines(vector<Vec2f>& lines, float lo, float hi);
void findLines_Horizonta(vector<Vec2f>& lines);
void findLines_Vertical(vector<Vec2f>& lines);


/* �������ҵ��������ֵtarget��ӽ�������ֱ��*/
void findHorizontaLinesNearTarget(vector<Vec2f>& lines, float target);
void findHorizontaLines_MaxInterval(vector<Vec2f>& lines);

void drawLines(Mat &image, vector<Vec2f> &lines);
void drawRectangles(Mat &image, vector<Rect>& rects);
void _calCutPosition_Horizontal(const Mat& canny_image, vector<float>& position, int count_threshold);
void _cut_Horizontal(const Mat& canny_image, Rect rect, vector<Rect>& rects, int threshold);
int cut_Horizontal(const Mat& canny_image, Rect rect, vector<Rect>& rects);
void _calcutPosition_Vertical(const Mat& canny_image, vector<float>& position, int count_threshold);
int cut_Vertical(const Mat& canny_image, Rect rect, vector<Rect>& rects);

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

Rect cutByRange(Rect rect, RangeLR range);

void stripRight(string &str);

void batchOCR(tesseract::TessBaseAPI& tess_ocr, const Mat& image, const vector<Rect>& areas, vector<string> &result);

RangeLR findEndPointsOfLine(Mat image, int idx);

/* �����ı���ֵ�����ֱ��*/
int detectLines(const Mat& src_image, vector<Vec2f>& lines);