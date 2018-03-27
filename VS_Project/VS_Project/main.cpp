#include "calibration.h"

using namespace std;

int main() {
	CameraCalibrator myCameraCalibrator;

	cout << "step 1. computing points... please wait" << endl;
#ifdef JPGInput //disable read image files as input datas, so No need setFilename() function
	myCameraCalibrator.setFilename();
#endif
	myCameraCalibrator.setBorderSize(Size(6, 4));   //set chessboard size is 6x4  
	if (myCameraCalibrator.addChessboardPoints())
	{
		cout << "No calibration datas, Terminate this process." << endl;
		waitKey();
		return -1;
	}
	cout << "step 1. compute finished." << endl;

	cout << "step 2. reading a distorted file and doing calibration..." << endl;
	Mat src = imread("RE_0222_0033.jpg", 0);
	Mat dst;
	myCameraCalibrator.calibrate(src, dst);
	cout << "step 2. calibration finished. Show undistorted image." << endl;

	imshow("Original Image", src);
	imshow("Undistorted Image", dst);
	remove("temp.jpg");
	imwrite("distorted_sample.jpg", src);
	imwrite("Undistorted_sample.jpg", dst);
	waitKey();

	return 0;
}