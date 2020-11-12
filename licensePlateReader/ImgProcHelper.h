#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

class ImgProcHelper
{
public:
	static bool isPlateVisible(int& plateContourIndex, std::vector<std::vector<cv::Point>> contours)
	{
		cv::Mat cutImg(cv::Scalar(0, 0, 0));
		int plate_search = 0;
		for (size_t i = 0; i < contours.size(); i++)
		{
			if (cv::arcLength(contours[i], true) > 100 && cv::contourArea(contours[i]) > 1500)
			{
				float width = cv::boundingRect(contours[i]).width;
				float height = cv::boundingRect(contours[i]).height;
				if (width / height > 3 && width / height < 5)
				{        // wyszukiwanie konturu o wymiarach tablicy
					//cv::drawContours(contoursImg, contours, i, cv::Scalar(0, 255, 0), 1);
					//cutImg = gray(boundingRect(contours[i]));
					//tablica_area = width * height;
					//plate_search = 1;
					plateContourIndex = i;
					return true;
				}
			}
		}
		return false;
	}

	static void cutPlateFromImg(int plateContourIndex, std::vector<std::vector<cv::Point>> contours, cv::Mat grayImg, cv::Mat& cutImg)
	{
		cutImg = grayImg(cv::boundingRect(contours[plateContourIndex]));
	}

	static int getImgArea(cv::Mat img)
	{
		return img.cols * img.rows;
	}

	static void sortContours(std::vector<std::vector<cv::Point>>& contours)
	{
		for (int i = 0; i < contours.size(); i++)           //sortowanie konturow znakow od lewej do prawej
		{
			for (int j = 1; j < contours.size() - i; j++)
			{
				if (boundingRect(contours[j - 1]).x > boundingRect(contours[j]).x)
				{
					swap(contours[j - 1], contours[j]);
				}
			}
		}
	}

	static bool isContourOutsidePreviousOne(std::vector<cv::Point> currentContour, std::vector<cv::Point> previousContour)
	{
		return (boundingRect(currentContour).x > boundingRect(previousContour).x + boundingRect(previousContour).width);
	}
};