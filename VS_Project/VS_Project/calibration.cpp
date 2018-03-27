#include "calibration.h"

#ifdef JPGInput //disable read image files as input datas, so No need setFilename() function
void CameraCalibrator::setFilename() {
	int count = 1;
	char FileIndex[10];
	char FileName[64];
	FILE *fp = NULL;

	m_filenames.clear();
#if 1 //add image files into list dynamically
	while (true)
	{
		sprintf_s(FileIndex, "%03d", count);
		strcpy_s(FileName, "ImageOut_");
		strcat_s(FileName, FileIndex);
		strcat_s(FileName, ".jpg");

		if (!fopen_s(&fp, FileName, "r"))
		{
			m_filenames.push_back(FileName);
			count++;
		}
		else
		{
			cout << "total read " << --count << " images" << endl;
			break;
		}
	}
#else //add files into list with fixed amount image files
	m_filenames.push_back("chessboard01.jpg");
	m_filenames.push_back("chessboard02.jpg");
	m_filenames.push_back("chessboard03.jpg");
	m_filenames.push_back("chessboard04.jpg");
	m_filenames.push_back("chessboard05.jpg");
	m_filenames.push_back("chessboard06.jpg");
	m_filenames.push_back("chessboard07.jpg");
	m_filenames.push_back("chessboard08.jpg");
	m_filenames.push_back("chessboard09.jpg");
	m_filenames.push_back("chessboard10.jpg");
	m_filenames.push_back("chessboard11.jpg");
	m_filenames.push_back("chessboard12.jpg");
	m_filenames.push_back("chessboard13.jpg");
	m_filenames.push_back("chessboard14.jpg");
#endif
}
#endif // #ifdef JPGInput

void CameraCalibrator::setBorderSize(const Size &borderSize) {
	m_borderSize = borderSize;
}

int CameraCalibrator::addChessboardPoints() {
	vector<Point2f> srcCandidateCorners;
	vector<Point3f> dstCandidateCorners;
	bool success;
	int successCount = 0, interval = 0;
	for (int i = 0; i<m_borderSize.height; i++) {                  //height is 4
		for (int j = 0; j<m_borderSize.width; j++) {               //width is 6
			dstCandidateCorners.push_back(Point3f((float)i, (float)j, 0.0f));    //initialize dstCandidateCorners
		}
	}
#ifdef AVIInput //chage to read datas from a avi file (named as input_video.avi)
	VideoCapture video("RE_0222.avi");
	if (!video.isOpened()) {
		return -1;
	}

	Size videoSize = Size((int)video.get(CV_CAP_PROP_FRAME_WIDTH), (int)video.get(CV_CAP_PROP_FRAME_HEIGHT));
	//namedWindow("video demo", CV_WINDOW_NORMAL);
	Mat videoFrame, image;
	while (true) {
		video >> videoFrame;
		if (!(interval++ % 10))
		{
			imwrite("temp.jpg", videoFrame);
			image = imread("temp.jpg", CV_LOAD_IMAGE_GRAYSCALE);
			if (image.empty()) {
				break;
			}

			success = findChessboardCorners(image, m_borderSize, srcCandidateCorners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);   //input image for findChessboardCorners must be an 8-bit grayscale or color image 
			if (success)
			{
				++successCount;
				TermCriteria param(TermCriteria::MAX_ITER + TermCriteria::EPS, 30, 0.1);
				cornerSubPix(image, srcCandidateCorners, Size(5, 5), Size(-1, -1), param);
				cout << "srcCandidateCorners.size() = " << srcCandidateCorners.size() << ", success count = " << successCount << endl;
				if (srcCandidateCorners.size() == m_borderSize.area()) {
					addPoints(srcCandidateCorners, dstCandidateCorners);
				}
				// --- draw corners (can disable) 
				drawChessboardCorners(image, m_borderSize, srcCandidateCorners, success);
				imshow("video frame", image);
			}

			if (successCount < 15)
				waitKey(10);
			else
				break;
		}
		else
			continue;
	}
	return 0;
#else //read input data from JPG images originally 
	Mat image;
	for (int i = 0; i<m_filenames.size(); i++)
	{
		image = imread(m_filenames[i], CV_LOAD_IMAGE_GRAYSCALE);
		imshow("input JPG image", image);
		success = findChessboardCorners(image, m_borderSize, srcCandidateCorners);
		if (success)
		{
			cout << "frame " << i << ", find all corners successfully" << endl;
			TermCriteria param(TermCriteria::MAX_ITER + TermCriteria::EPS, 30, 0.1);
			cornerSubPix(image, srcCandidateCorners, Size(5, 5), Size(-1, -1), param);
			if (srcCandidateCorners.size() == m_borderSize.area())
			{
				addPoints(srcCandidateCorners, dstCandidateCorners);
			}
		}

		waitKey(33);
	}
	return 0;
#endif //#ifdef AVIInput
}

void CameraCalibrator::addPoints(const vector<Point2f> &srcCorners, const vector<Point3f> &dstCorners) {
	m_srcPoints.push_back(srcCorners);
	m_dstPoints.push_back(dstCorners);
}

void CameraCalibrator::calibrate(const Mat &src, Mat &dst) {
	Size imageSize = src.size();
	Mat cameraMatrix, distCoeffs, map1, map2;
	vector<Mat> rvecs, tvecs;
	//Finds the camera intrinsic and extrinsic parameters from several views of a calibration pattern.
	//cameraMatrix: 3x3 matrix A in sm' = A[R|t]M'
	calibrateCamera(m_dstPoints, m_srcPoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
	cout << "# Camera Matrix" << endl;
	cout << cameraMatrix << endl;
	cout << "# distCoeffs" << endl;
	cout << distCoeffs << endl;
	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), Mat(), imageSize, CV_32F, map1, map2);
	remap(src, dst, map1, map2, INTER_LINEAR);
}