#include"ImgProcHelper.h"

int main(int argc, char** argv)
{
	cv::Mat templateImg, grayTemplateImage;

	templateImg = cv::imread("C:/Users/Kalin/Desktop/wma_materialy//wzory_dobre.png"); 
	cv::cvtColor(templateImg, grayTemplateImage, cv::COLOR_BGR2GRAY);

	std::cout << ImgProcHelper::readLicensePlate("C:/Users/Kalin/Desktop/wma_materialy/tab1.jpg", grayTemplateImage) << std::endl;
	
	return 0;
}