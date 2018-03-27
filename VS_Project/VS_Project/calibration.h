#pragma once
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#if 1
#define AVIInput
#else
#define JPGInput
#endif

using namespace cv;
using namespace std;

class CameraCalibrator {
private:
#ifdef JPGInput //disable read image files as input datas, so No need setFilename() function
	vector<string> m_filenames;
#endif
	Size m_borderSize;
	vector<vector<Point2f> > m_srcPoints;
	vector<vector<Point3f> > m_dstPoints;
public:
#ifdef JPGInput //disable read image files as input datas, so No need setFilename() function
	void setFilename();
#endif
	void setBorderSize(const Size &borderSize);
	int addChessboardPoints();
	void addPoints(const vector<Point2f> &imageCorners, const vector<Point3f> &objectCorners);
	void calibrate(const Mat &src, Mat &dst);
};