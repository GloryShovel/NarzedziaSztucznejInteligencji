#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;
using namespace cv;

//Args: pointOne pointTwo pointWithAngle
double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

int main()
{
	bool showCannyWindows = true;

	int canny_a = 76, canny_b = 106, photoCounter = 0;
	bool isPhotoSaved = false;

	namedWindow("prostokaty", cv::WINDOW_AUTOSIZE);
	createTrackbar("A", "prostokaty", &canny_a, 255);
	createTrackbar("B", "prostokaty", &canny_b, 255);

	VideoCapture camera(0);

	while (waitKey(1) != 27)
	{
		//getting camera info
		Mat frame, frame0, frameBw, frameCanny;
		camera >> frame;
		frame0 = frame.clone();

		//converting to gray for Cany setup
		cvtColor(frame, frameBw, COLOR_BGR2GRAY);
		if(showCannyWindows){
			imshow("bw", frameBw);
		}

		//equalizing distribution of intensity
		equalizeHist(frameBw, frameBw);
		if (showCannyWindows) {
			imshow("bw_hist", frameBw);
		}
		
		//Edge detector
		Canny(frameBw, frameCanny, canny_a, canny_b, 3);
		if (showCannyWindows) {
			imshow("bw_Canny", frameCanny);
		}

		//Denoising and edge detection
		static auto ellipse = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
		morphologyEx(frameBw, frameBw, MORPH_CLOSE, ellipse);
		morphologyEx(frameBw, frameBw, MORPH_OPEN, ellipse);
		Canny(frameBw, frameCanny, canny_a, canny_b, 3);
		if (showCannyWindows) {
			imshow("bw_Canny_2", frameCanny);
		}
		//Making edges thick ;)
		static auto ellipse_33 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		morphologyEx(frameCanny, frameCanny, MORPH_DILATE, ellipse_33);
		if (showCannyWindows) {
			imshow("bw_Canny_2_dil", frameCanny);
		}
		
		//searching for contours, aproximating them, selecting ones with only 4 verts and saving them in contours_4
		vector<vector<Point>> contours, contours_4;
		findContours(frameCanny, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
		for (int i = 0; i < contours.size(); i++)
		{
			approxPolyDP(contours[i], contours[i], 10, true);
			if (contours[i].size() == 4)
			{
				double amax = 0;
				for (int j = 0; j < 4; j++)
				{
					//Selecting only squares 
					double a = fabs(angle(contours[i][j], contours[i][(j + 2) % 4], contours[i][(j + 1) % 4]));
					amax = max(a, amax);
				}
				if (amax < 0.4)
					contours_4.push_back(contours[i]);
			}
		}

		//Drawing contours
		for (int i = 0; i < contours_4.size(); i++)
		{
			drawContours(frame, contours_4, i, Scalar(0, 0, 255));
		}

		//Transforming biggest of squares to normal perspective and showing it in window
		if (contours_4.size() > 0)
		{
			//sorting contours_4 and draw them
			sort(contours_4.begin(), contours_4.end(), 
				[](auto& a, auto& b) {
					return contourArea(a, false) > contourArea(b, false);
				});
			drawContours(frame, contours_4, 0, Scalar(0, 255, 0), 2);

			Mat dstMat(Size(300, 200), CV_8UC3);
			vector<Point2f> src = { {0, 0}, {(float)dstMat.cols, 0}, {(float)dstMat.cols, (float)dstMat.rows}, {0, (float)dstMat.rows} };
			vector<Point2f> dst;
			for (auto p : contours_4[0])
				dst.push_back(Point2f(p.x, p.y));

			//trasnforming square to proper perspective
			auto wrap_mtx = getPerspectiveTransform(dst, src);
			warpPerspective(frame0, dstMat, wrap_mtx, Size(dstMat.cols, dstMat.rows));

			//Checkpoint XD
			if (true) {
				imshow("RESULT", dstMat);
			}

			//Checking for marker on dstMat by finding color-red
			Mat result, redMask, redNegMask;
			vector<vector<Point>> redContours;
			int dilation_size = 5;
			auto structElem = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1), cv::Point(dilation_size, dilation_size));

			cvtColor(dstMat, redMask, cv::COLOR_BGR2HSV);
			inRange(redMask, cv::Scalar(0, 23, 44), cv::Scalar(21, 255, 255), redNegMask);
			morphologyEx(redNegMask, redNegMask, cv::MORPH_CLOSE, structElem);
			morphologyEx(redNegMask, redNegMask, cv::MORPH_OPEN, structElem);

			if (showCannyWindows) {
				imshow("Mask of image", redNegMask);
			}

			//searches for countours and reduces number of points needed to draw them
			findContours(redNegMask, redContours, cv::RETR_LIST, cv::CHAIN_APPROX_TC89_KCOS);

			if (redContours.size()) {
				//Gathering info about red marker
				Point begin, end;
				Rect r = boundingRect(redContours.at(0));
				begin.x = r.x;
				begin.y = r.y;
				end.x = r.x + r.width;
				end.y = r.y + r.height;

				if (showCannyWindows) {
					putText(dstMat, "X", begin, cv::FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0), 2);
					putText(dstMat, "X", end, cv::FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 2);
					//line(dstMat, begin, end, Scalar(0,0,0), 2);
				}

				//Rotating result image so that red marker will be in right bottom corner
				if (begin.x < dstMat.size().width/2) {
					if (begin.y < dstMat.size().height/2) {
						cv::rotate(dstMat, dstMat, cv::ROTATE_90_COUNTERCLOCKWISE);
						cv::rotate(dstMat, dstMat, cv::ROTATE_90_COUNTERCLOCKWISE);
					}
					else {
						cv::rotate(dstMat, dstMat, cv::ROTATE_90_COUNTERCLOCKWISE);
					}
				}
				else
				{
					if (begin.y < dstMat.size().height/2)
					{
						cv::rotate(dstMat, dstMat, cv::ROTATE_90_CLOCKWISE);
					}
				}
				cout << begin.x << endl;
				//Saving in file and letting know that it happened
				if (!isPhotoSaved) {
					if (abs(begin.x - end.x) < 20 && abs(begin.y - end.y) > 100) {
					}
					else
					{
						transpose(dstMat, dstMat);
					}

					imwrite("zdj" + to_string(photoCounter) + ".png", dstMat);
					putText(frame, "Zjêcie zosta³o wykonane", { 0,20 }, cv::FONT_HERSHEY_PLAIN, 2, { 255,0,0 }, 2);
					photoCounter++;

					//For series of photos to choose from
					if (photoCounter > 10) {
						isPhotoSaved = true;
					}
				}

				if (showCannyWindows){
					imshow("Is ok", dstMat);
				}
			}
		}

		//UI element letting know that photo have been alredy saved
		if (isPhotoSaved) {
			putText(frame, "Zjecia zostaly wykonane", { 0,20 }, cv::FONT_HERSHEY_PLAIN, 2, { 0,255,0 }, 2);
		}

		imshow("frame_from_cam", frame);
	}
	return 0;
}