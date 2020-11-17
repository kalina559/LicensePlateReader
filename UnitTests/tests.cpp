#define CATCH_CONFIG_MAIN
#include "libs/catch/catch.hpp"
#include"licensePlateReader/LicensePlateReader/ImgProcHelper.h"
#include<iostream>

TEST_CASE("Still license plates images")
{
	cv::Mat templateImg, grayTemplateImage;

	templateImg = cv::imread("C:/Users/Kalin/Desktop/wma_materialy//wzory_dobre.png");
	cv::cvtColor(templateImg, grayTemplateImage, cv::COLOR_BGR2GRAY);

	REQUIRE(ImgProcHelper::readLicensePlate("C:/Users/Kalin/Desktop/wma_materialy/tab1.jpg", grayTemplateImage) == "WE984KV");
	REQUIRE(ImgProcHelper::readLicensePlate("C:/Users/Kalin/Desktop/wma_materialy/tab2.jpg", grayTemplateImage) == "SB8903R");
	REQUIRE(ImgProcHelper::readLicensePlate("C:/Users/Kalin/Desktop/wma_materialy/tab3.jpg", grayTemplateImage) == "HPHYO35");
	REQUIRE(ImgProcHelper::readLicensePlate("C:/Users/Kalin/Desktop/wma_materialy/tab4.jpg", grayTemplateImage) == "PZ596LT");
	REQUIRE(ImgProcHelper::readLicensePlate("C:/Users/Kalin/Desktop/wma_materialy/tab5.jpg", grayTemplateImage) == "P0BUDKA");
	REQUIRE(ImgProcHelper::readLicensePlate("C:/Users/Kalin/Desktop/wma_materialy/tab6.jpg", grayTemplateImage) == "BL19957");
	REQUIRE(ImgProcHelper::readLicensePlate("C:/Users/Kalin/Desktop/wma_materialy/tab7.jpg", grayTemplateImage) == "EL929P0");
	REQUIRE(ImgProcHelper::readLicensePlate("C:/Users/Kalin/Desktop/wma_materialy/tab8.jpg", grayTemplateImage) == "PO48788");
}
