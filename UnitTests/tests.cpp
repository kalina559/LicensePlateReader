#define CATCH_CONFIG_MAIN
#include "libs/catch/catch.hpp"
#include"licensePlateReader/LicensePlateReader/ImgProcHelper.h"
#include<iostream>
#include<experimental/filesystem>

TEST_CASE("Still license plates images")
{
	cv::Mat templateImg, grayTemplateImage;

	std::string projectPath = std::experimental::filesystem::current_path().u8string();

	templateImg = cv::imread(projectPath + "/images/templateCharacters.jpg");
	cv::cvtColor(templateImg, grayTemplateImage, cv::COLOR_BGR2GRAY);
	
	REQUIRE(ImgProcHelper::readLicensePlate(projectPath + "/images/license1.jpg", grayTemplateImage) == "WE984KV");
	REQUIRE(ImgProcHelper::readLicensePlate(projectPath + "/images/license2.jpg", grayTemplateImage) == "SB8903R");
	REQUIRE(ImgProcHelper::readLicensePlate(projectPath + "/images/license3.jpg", grayTemplateImage) == "HPHY035");
	REQUIRE(ImgProcHelper::readLicensePlate(projectPath + "/images/license4.jpg", grayTemplateImage) == "PZ596LT");
	REQUIRE(ImgProcHelper::readLicensePlate(projectPath + "/images/license5.jpg", grayTemplateImage) == "POBUDKA");
	REQUIRE(ImgProcHelper::readLicensePlate(projectPath + "/images/license6.jpg", grayTemplateImage) == "BL19957");
	REQUIRE(ImgProcHelper::readLicensePlate(projectPath + "/images/license7.jpg", grayTemplateImage) == "EL929P0");
	REQUIRE(ImgProcHelper::readLicensePlate(projectPath + "/images/license8.jpg", grayTemplateImage) == "PO48788");
	REQUIRE(ImgProcHelper::readLicensePlate(projectPath + "/images/forest.jpg", grayTemplateImage) == "No license plate was found in the image");
}
