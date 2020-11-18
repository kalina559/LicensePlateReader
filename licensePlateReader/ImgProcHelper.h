#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#define templateString "ABCDEFGHIJKLMNOPRSTUVWXYZ0123456789"

class ImgProcHelper
{
public:
	static std::string readLicensePlate(std::string path, cv::Mat& grayTemplateImage)
	{
		cv::Mat inputImg, gray;
		std::vector<std::vector<cv::Point> > contours;

		inputImg = cv::imread(path);
		cvtColor(inputImg, gray, cv::COLOR_BGR2GRAY);

		getContoursFromGrayScale(gray, contours, plate);
		cv::Mat plateImg(inputImg.size(), CV_64FC3, cv::Scalar(0, 0, 0));

		std::vector<int> possiblePlates = getPossiblePlates(contours);

		while (!possiblePlates.empty())
		{
			cutPlateFromImg(possiblePlates.back(), contours, gray, plateImg);
			if (getLettersFromPlate(grayTemplateImage, plateImg).length() == 7)
			{
				return getLettersFromPlate(grayTemplateImage, plateImg);
			}
			else
			{
				cv::Mat plateImg(inputImg.size(), CV_64FC3, cv::Scalar(0, 0, 0));
				possiblePlates.pop_back();
			}
		}

		return "No license plate was found in the image";
	}

	static bool isPlateVisible(int& plateContourIndex, std::vector<std::vector<cv::Point>> contours)
	{
		for (size_t i = 0; i < contours.size(); i++)
		{
			if (cv::arcLength(contours[i], true) > 100 && cv::contourArea(contours[i]) > 1500)
			{
				float width = cv::boundingRect(contours[i]).width;
				float height = cv::boundingRect(contours[i]).height;
				if (width / height > 3 && width / height < 5)
				{        
					plateContourIndex = i;
					return true;
				}
			}
		}
		return false;
	}

	static std::vector<int> getPossiblePlates(std::vector<std::vector<cv::Point>> contours)
	{
		std::vector<int> possiblePlatesIndexes;
		for (size_t i = 0; i < contours.size(); i++)
		{
			float width = cv::boundingRect(contours[i]).width;
			float height = cv::boundingRect(contours[i]).height;
			if (width / height > 3 && width / height < 5)
			{
				possiblePlatesIndexes.push_back(i);
			}
		}
		return possiblePlatesIndexes;
	}

	static void sortContours(std::vector<std::vector<cv::Point>>& contours)
	{
		for (int i = 0; i < contours.size(); i++)           //sort the contours from right to left
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

	enum expectedContour
	{
		plate, character
	};
	static void getContoursFromGrayScale(cv::Mat& inputImg, std::vector<std::vector<cv::Point> >& contours, expectedContour expContour)
	{
		
		cv::Mat threshImg(cv::Scalar(0, 0, 0));
		threshold(inputImg, threshImg, 110, 255, cv::THRESH_BINARY_INV);

		cv::Mat morphedImg(cv::Scalar(0, 0, 0));
		int kernelSize;
		if (expContour == plate)
		{
			kernelSize = 4;
		}
		else
		{
			kernelSize = 2;
		}
		cv::Mat kernel = getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
		morphologyEx(threshImg, morphedImg, cv::MORPH_CLOSE, kernel);

		cv::Mat contoursImg(inputImg.size(), CV_64FC3, cv::Scalar(0, 0, 0));

		cv::Mat cannyOutputImg(cv::Scalar(0, 0, 0));

		Canny(morphedImg, cannyOutputImg, 60, 60 * 3);
		if (expContour == plate)
		{
			findContours(cannyOutputImg, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
		}
		else
		{
			findContours(cannyOutputImg, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		}
		
	}	

	static std::string getLettersFromPlate(cv::Mat& grayTemplateImg, cv::Mat& plateImg)
	{
		std::vector<std::vector<cv::Point>> contours;
		getContoursFromGrayScale(plateImg, contours, character);
		sortContours(contours);		

		cv::Mat checkedCharacter;
		std::string licensePlate = "", letter;

		std::vector<cv::Point> previousContour;

		for (size_t i = 0; i < contours.size(); i++)
		{
			float width = boundingRect(contours[i]).width;
			float height = boundingRect(contours[i]).height;
			if (isContourACharacter(width, height, plateImg, contours[i], previousContour))
			{
				previousContour = contours[i];

				checkedCharacter = plateImg(boundingRect(contours[i]));        // cutting a character out of the plate image
				while (checkedCharacter.rows > 28)  //the row with biggest character size is approximately this height
				{					
					pyrDown(checkedCharacter, checkedCharacter);
				}
				letter = findCorrespondingLetter(findMatchingCharacterCoordinateX(grayTemplateImg, checkedCharacter), contours[i]);
				licensePlate = licensePlate + letter;
			}
		}

		return licensePlate;
	}

	static char findCorrespondingLetter(int x,  std::vector<cv::Point> contour)
	{               
		int k = 0;
		while (!(x >= 1 + k * 33 && x <= 27 + k * 33))
		{
			++k;
		}
		if (templateString[k] == '0' || templateString[k] == 'O' || templateString[k] == 'U') // U and 0 are often mixed up, so the ratio of contour area to boundingRect area is checked to determine which one is it
		{
			double circularity = 4 * 3.14 * cv::contourArea(contour) / (cv::arcLength(contour, true) * cv::arcLength(contour, true));     // for a perfect circle it circularity = 1, the less the contour resembles a circle, the lower the value
			//std::cout << "initial char: " << templateString[k] << " ratio: " << circularity << std::endl;
			if (circularity < 0.8)
			{
				if (circularity < 0.4)
				{
					return 'U';
				}
				return '0';
			}
			return 'O';
		}		
		return templateString[k];
	}

	static int findMatchingCharacterCoordinateX(cv::Mat& grayTemplateImage, cv::Mat& checkedCharacter)
	{
		double minValue, maxValue;
		cv::Point minLocation, maxLocation, matchLocation;

		cv::Mat resultMap;
		matchTemplate(grayTemplateImage, checkedCharacter, resultMap, cv::TM_SQDIFF_NORMED);
		minMaxLoc(resultMap, &minValue, &maxValue, &minLocation, &maxLocation, cv::Mat());
		matchLocation = minLocation;

		return matchLocation.x;
	}

	static bool isContourACharacter(float width, float height, cv::Mat& cutImg, std::vector<cv::Point>& currentContour, std::vector<cv::Point>& previousContour)
	{
		return (width / height > 0.3 && width / height < 0.75 && double(width * height) > 0.015 * getImgArea(cutImg) && isContourOutsidePreviousOne(currentContour, previousContour));
	}

	static void cutPlateFromImg(int plateContourIndex, std::vector<std::vector<cv::Point>> contours, cv::Mat grayImg, cv::Mat& cutImg)
	{
		cutImg = grayImg(cv::boundingRect(contours[plateContourIndex]));
	}

	static int getImgArea(cv::Mat img)
	{
		return img.cols * img.rows;
	}
};