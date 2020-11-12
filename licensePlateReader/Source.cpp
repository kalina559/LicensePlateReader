#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include"ImgProcHelper.h"

using namespace std;
using namespace cv;


string szukanie_znaku(int x, int y, int z = 0) {               //funkcja przeszukujaca tablice znakow
	int k = 0;
	while (!(x >= 1 + k * 33 && x <= 27 + k * 33)) {
		k++;
	}
	if (k == 0)
		return "A";
	if (k == 1)
		return "B";
	if (k == 2)
		return "C";
	if (k == 3)
		return "D";
	if (k == 4)
		return "E";
	if (k == 5)
		return "F";
	if (k == 6)
		return "G";
	if (k == 7)
		return "H";
	if (k == 8)
		return "I";
	if (k == 9)
		return "J";
	if (k == 10)
		return "K";
	if (k == 11)
		return "L";
	if (k == 12)
		return "M";
	if (k == 13)
		return "N";
	if (k == 14)
		return "O";
	if (k == 15)
		return "P";
	if (k == 16)
		return "R";
	if (k == 17)
		return "S";
	if (k == 18)
		return "T";
	if (k == 19)
		return "U";
	if (k == 20)
		return "V";
	if (k == 21)
		return "W";
	if (k == 22)
		return "X";
	if (k == 23)
		return "Y";
	if (k == 24)
		return "Z";
	if (k == 25 && z == 0)
		return "0";
	if (k == 25 && z == 1)
		return "U";
	if (k == 26)
		return "1";
	if (k == 27)
		return "2";
	if (k == 28)
		return "3";
	if (k == 29)
		return "4";
	if (k == 30)
		return "5";
	if (k == 31)
		return "6";
	if (k == 32)
		return "7";
	if (k == 33)
		return "8";
	if (k == 34)
		return "9";
	return "X";
}


int main(int argc, char** argv)
{


		Mat inputImg = imread("C:/Users/Kalin/Desktop/wma materialy/tab1.jpg");
		Mat gray;

		cvtColor(inputImg, gray, COLOR_BGR2GRAY);

		Mat threshImg(Scalar(0, 0, 0));
		threshold(gray, threshImg, 110, 255, THRESH_BINARY_INV);

		Mat morphedImg(Scalar(0, 0, 0));
		Size kernelSize(4, 4);
		Mat kernel = getStructuringElement(MORPH_RECT, kernelSize);
		morphologyEx(threshImg, morphedImg, MORPH_CLOSE, kernel);

		Mat contoursImgTest(inputImg.size(), CV_64FC3, Scalar(0, 0, 0));
		Mat contoursImg(inputImg.size(), CV_64FC3, Scalar(0, 0, 0));
		vector<vector<Point> > contours;
		Mat canny_output(Scalar(0, 0, 0));

		Canny(morphedImg, canny_output, 60, 60 * 3);
		findContours(canny_output, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
		drawContours(contoursImgTest, contours, -1, Scalar(0, 255, 0), 1);

		Mat cutImg(Scalar(0, 0, 0));

		int plateContourIndex = 0;
		if (ImgProcHelper::isPlateVisible(plateContourIndex, contours))
		{
			ImgProcHelper::cutPlateFromImg(plateContourIndex, contours, gray, cutImg);


			Mat cannyImage;

			Canny(cutImg, cannyImage, 60, 60 * 3);
			findContours(cannyImage, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
			drawContours(contoursImg, contours, -1, Scalar(0, 255, 0), 1);

			ImgProcHelper::sortContours(contours);

			Mat wzor(Scalar(0, 0, 0)), gray_wzor(Scalar(0, 0, 0));

			wzor = imread("C:/Users/Kalin/Desktop/wma materialy//wzory_dobre.png");          //wzory znakow
			cvtColor(wzor, gray_wzor, COLOR_BGR2GRAY);

			double minVal, maxVal;
			Point minLoc, maxLoc, matchLoc;
			Mat img_display(Scalar(0, 0, 0));
			wzor.copyTo(img_display);
			Mat checkedCharacter(Scalar(0, 0, 0));
			string licensePlate = "";
			string znak;

			std::vector<cv::Point> previousContour;

			for (size_t i = 0; i < contours.size(); i++)
			{
				float width = boundingRect(contours[i]).width;
				float height = boundingRect(contours[i]).height;
				if (width / height > 0.3 && width / height < 0.75 && width * height > 0.015 * ImgProcHelper::getImgArea(cutImg) && ImgProcHelper::isContourOutsidePreviousOne(contours[i], previousContour))
				{
					rectangle(contoursImg, boundingRect(contours[i]).tl(), boundingRect(contours[i]).br(), Scalar(0, 0, 255), 1);
					previousContour = contours[i];

					checkedCharacter = cutImg(boundingRect(contours[i]));      //porownywanie znaku (templ) z tablica wzorow (gray_wzor)
					while (checkedCharacter.rows > 25)
					{                            //25 to mniej wiecej wysokosc najwiekszego rzedu znakow w tablicy wzorow
						pyrDown(checkedCharacter, checkedCharacter);
					}
					int rezultat_cols = wzor.cols - checkedCharacter.cols + 1;
					int rezultat_rows = wzor.rows - checkedCharacter.rows + 1;
					Mat rezultat(rezultat_rows, rezultat_cols, CV_32FC1);
					matchTemplate(gray_wzor, checkedCharacter, rezultat, TM_SQDIFF_NORMED);
					minMaxLoc(rezultat, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
					matchLoc = minLoc;
					rectangle(img_display, matchLoc, Point(matchLoc.x + checkedCharacter.cols, matchLoc.y + checkedCharacter.rows), Scalar(0, 0, 0), 2);

					znak = szukanie_znaku(matchLoc.x, matchLoc.y);
					if (znak == "0" && (contourArea(contours[i])) / (width * height) < 0.6)
					{
						// U czesto sie myli z 0 wiec sprawdzam pow. konturu/pow. prostokata, dla 0 jest to ok. 0.8, a dla U <0.4
						znak = szukanie_znaku(matchLoc.x, matchLoc.y, 1);
					}
					licensePlate = licensePlate + znak;
					imshow("przyporzadkowanie znaku do wzoru", img_display);     //pokazuje przyporzadkowanie danego znaku do wzoru
					waitKey(0);

				}
			}
			cout << "rejestracja: " << licensePlate;
			putText(inputImg, licensePlate, Point(10, 50), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 2);
			imshow("zdjecie: ", inputImg);
			waitKey();
			destroyAllWindows();

		}
		else {                  //jesli nie wykryto tablicy
			putText(inputImg, "Nie wykryto zadnej tablicy", Point(10, 50), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 2);
			imshow("zdjecie: ", inputImg);
			waitKey();
			destroyAllWindows();
		}

		

	
	return 0;

}