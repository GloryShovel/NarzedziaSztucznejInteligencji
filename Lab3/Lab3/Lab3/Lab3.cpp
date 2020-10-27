// Lab3.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//
#include <iostream>
#include <windows.h>
#include <MMSystem.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

//Takes two points and returns them as vector<int> from point 0,0
cv::Point makeVector(cv::Point a, cv::Point b) {
	cv::Point result;
	if (a.x > b.x) {
		result = {a.x - b.x, a.y - b.y};
	}
	else {
		result = {b.x - a.x, b.y - a.y };
	}

	//Left it here in case someone want to see how good this funcion is ;)
	std::cout << "a:" + std::to_string(a.x) +","+ std::to_string(a.y) + " b:" + std::to_string(b.x) + "," + std::to_string(b.y) + " " " result:" + std::to_string(result.x) + "," + std::to_string(result.y) + " " << std::endl;
	
	return result;
}

int main()
{
	//Dev Settings
	bool showAllWindows = false, showContours = false, showConectingLine = false;

	bool isMusicPlaing = false;

	std::vector<std::vector<cv::Point>> contours;

	int loRange[3] = { 76, 88, 76 };
	int hiRange[3] = { 119, 255, 255 };

	namedWindow("bars", cv::WINDOW_AUTOSIZE);
	cv::createTrackbar("loRange0", "bars", &(loRange[0]), 255);
	cv::createTrackbar("loRange1", "bars", &(loRange[1]), 255);
	cv::createTrackbar("loRange2", "bars", &(loRange[2]), 255);
	cv::createTrackbar("hiRange0", "bars", &(hiRange[0]), 255);
	cv::createTrackbar("hiRange1", "bars", &(hiRange[1]), 255);
	cv::createTrackbar("hiRange2", "bars", &(hiRange[2]), 255);

	cv::VideoCapture cap(0);
	cv::Mat frame, frameMask, frameNegMask;

	class MyObject
	{
	public:
		int maxCount;
		std::vector<cv::Point> pos;
		cv::Point getP()
		{
			cv::Point sump = std::accumulate(pos.begin(), pos.end(), cv::Point(0, 0));
			sump.x /= pos.size();
			sump.y /= pos.size();
			return sump;
		}
		void addPoint(cv::Point p)
		{
			pos.push_back(p);
			if (pos.size() > maxCount)
			{
				pos = std::vector<cv::Point>(pos.begin() + 1, pos.end());
			}
		}
		void addEmpty()
		{
			if (pos.size() > 0)
			{
				pos = std::vector<cv::Point>(pos.begin() + 1, pos.end());
			}
		}
	};

	MyObject firstObject, secondObject;
	firstObject.maxCount = 10;
	secondObject.maxCount = 10;

	while (cv::waitKey(1) != 27)
	{
		int dilation_size = 5;
		auto structElem = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1), cv::Point(dilation_size, dilation_size));

		cap >> frame;
		//cv::flip(frame, frame, 1);
		cv::cvtColor(frame, frameMask, cv::COLOR_BGR2HSV);
		cv::inRange(frameMask, cv::Scalar(loRange[0], loRange[1], loRange[2]), cv::Scalar(hiRange[0], hiRange[1], hiRange[2]), frameNegMask);

		//To smooth out negative mask
		cv::morphologyEx(frameNegMask, frameNegMask, cv::MORPH_CLOSE, structElem);
		cv::morphologyEx(frameNegMask, frameNegMask, cv::MORPH_OPEN, structElem);

		//searches for countours and reduces number of points needed to draw them
		cv::findContours(frameNegMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_TC89_KCOS);

		//sort my contours
		sort(contours.begin(), contours.end(), [](auto& a, auto& b) {
			return contourArea(a, false) > contourArea(b, false);
		});

		//loop throught all contours on screen
		for (int i = 0; i < contours.size(); i++)
		{
			cv::approxPolyDP(contours.at(i), contours.at(i), 10, true);

			if (showContours) {
				drawContours(frame, contours, i, { 0, 0, 255, 255 });

				//ammount of points in countour
				auto txtpos = contours.at(i).at(0);
				putText(frame, std::to_string(contours.at(i).size()), txtpos, cv::FONT_HERSHEY_PLAIN, 2, { 0, 0, 255, 255 });

				//countour area
				txtpos.y += 30;
				putText(frame, std::to_string(contourArea(contours.at(i), false)), txtpos, cv::FONT_HERSHEY_PLAIN, 2, { 0, 0, 255, 255 });

				//Countour ID
				txtpos.y -= 60;
				putText(frame, std::to_string(i), txtpos, cv::FONT_HERSHEY_PLAIN, 2, { 0, 0, 255, 255 });
			}
		}

		//calculate center of contours and draw line between them
		if (contours.size())
		{
			cv::Point avg;
			cv::Rect r = cv::boundingRect(contours.at(0));
			avg.x = r.x + r.width / 2;
			avg.y = r.y + r.height / 2;
			firstObject.addPoint(avg);
			//putText(frame, "0", avg, cv::FONT_HERSHEY_PLAIN, 2, {0, 255, 255, 255});

			if (contours.size() > 1) {
				r = cv::boundingRect(contours.at(1));
				avg.x = r.x + r.width / 2;
				avg.y = r.y + r.height / 2;
				secondObject.addPoint(avg);

				//Draw line between two center points
				if (showConectingLine) {
					cv::line(frame, firstObject.getP(), secondObject.getP(), cv::Scalar(0,0,0), 2);
				}
			}
			else
			{
				secondObject.addEmpty();
			}
		}
		else
		{
			firstObject.addEmpty();
			secondObject.addEmpty();
		}

		if (firstObject.pos.size() > 1) {
			putText(frame, "X", firstObject.getP(), cv::FONT_HERSHEY_PLAIN, 2, { 255, 0, 255, 255 }, 3);

			//std::vector<std::vector<cv::Point>>ctrs = { firstObject.pos };
			//drawContours(frame, ctrs, 0, { 255, 0, 255, 255 });
		}

		if (secondObject.pos.size() > 1) {
			putText(frame, "X", secondObject.getP(), cv::FONT_HERSHEY_PLAIN, 2, { 255, 0, 255, 255 }, 3);

			//std::vector<std::vector<cv::Point>>ctrs = { secondObject.pos };
			//drawContours(frame, ctrs, 0, { 255, 0, 255, 255 });
		}

		//THE SPECIAL SECRET THAT ACTIVATES WHEN YOU ALIGN TWO MIDDLE POINTS HORIZONTALY
		if (contours.size() > 1) {
			 cv::Point vector=  makeVector(firstObject.getP(), secondObject.getP());

			 if (abs(vector.y) < 15 ) {
				 //std::cout << "Seems oki" << std::endl;
				 if (!isMusicPlaing) {
					 PlaySound(TEXT("doom_e1m1.wav"), NULL, SND_FILENAME | SND_ASYNC);
					 isMusicPlaing = true;
				 }
			 }
			 else {
				 PlaySound(NULL, NULL, SND_FILENAME);
				 isMusicPlaing = false;
			 }
		}

		//Drawing windows
		cv::imshow("Main camera", frame);
		if (showAllWindows) {
			cv::imshow("Color Convert", frameMask);
			cv::imshow("Negative Mask", frameNegMask);
		}
	}
}