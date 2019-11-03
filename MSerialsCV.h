#pragma once
#include <vector>
#include <string>
#include <list>
#include "common.h"
#include "opencv.hpp"
#include "HalconCpp.h"

#ifndef __DLL__IMAGE__
#define __DLL__IMAGE__
//C++导出 vc14平台集
//#ifndef CVDLL_API
#define CVDLL_API __declspec(dllexport)
//#else
//#define CVDLL_API __declspec(dllimport)
//#endif

CVDLL_API std::string CvGetVersion();
//数控送料
//CVDLL_API HalconCpp::HObject & CvReadDxf(const char *fileName, int64 winHandle = 0);
//CVDLL_API void CvSetPara(int Minh = 1, int Maxh = 100, int Minw = 1, int Maxw = 100);
//CVDLL_API HalconCpp::HObject & CvReLoadPara(int64 winHandle = 0);

CVDLL_API double CvSquareSize(cv::Mat mat, Control_Var & Ctrl_Var);																					//返回标定板对角线像素长度
CVDLL_API double CvCabli(double corner_size, Control_Var & Ctrl_Var, std::string & error_info, double ThresHold = 60, std::string dir = "CABLI");	//进行标定
CVDLL_API void CvCvtColor(const cv::Mat & org, cv::Mat & dst, int sel);																				//黑白彩色转换
CVDLL_API void CvLoadModelContours(cv::String FileName, Control_Var & Ctrl_Var);																	//功能 读取FileName的图片，将轮廓放在Ctrl var的modelcontours里面 并且图片存在Ctrl_Var Buffer里面
CVDLL_API bool __stdcall isFillMode(Control_Var &Ctrl_Var);
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsHorizentalEx(cv::Mat & InputArray, const std::vector<std::vector<cv::Point>> &ModleContour, Control_Var& Ctrl_Var);
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsVerticalEx(cv::Mat & InputArray, const std::vector<std::vector<cv::Point>> &ModleContour, Control_Var& Ctrl_Var);
//新增函数，分别是横向排列和横向扩充的意思
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsHorizentalAIFillEx(cv::Mat &InputArray, const std::vector<std::vector<cv::Point>> &model_contours, Control_Var& Ctrl_Var);
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsHorizentalAIFillSingle(cv::Mat & InputArray, const std::vector<std::vector<cv::Point>> &ModleContour, Control_Var& Ctrl_Var);




//以下函数拟定废弃
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsHorizentalAIFill(cv::Mat &InputArray, const std::vector<std::vector<cv::Point>> &model_contours, Control_Var& Ctrl_Var);				//智能横向
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsHorizentalAI(cv::Mat &InputArray, const std::vector<std::vector<cv::Point>> &model_contours, Control_Var& Ctrl_Var);				//智能横向
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsHorizental(cv::Mat &InputArray, const std::vector<std::vector<cv::Point>> &model_contours, Control_Var& Ctrl_Var);				//横向
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsCircleVerticalAI(cv::Mat & InputArray, const std::vector<std::vector<cv::Point>> &model_contours, Control_Var& Ctrl_Var);		//智能纵向
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsCircleVertical(cv::Mat &InputArray, const std::vector<std::vector<cv::Point>> &model_contours, Control_Var& Ctrl_Var);			//纵向
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsDoubleHorizental(cv::Mat &InputArray, const std::vector<std::vector<cv::Point>> &model_contours, Control_Var& Ctrl_Var);			//双面横向 不必要
CVDLL_API std::list<std::vector<cv::Point>> __stdcall CvGeAllPointsDoubleVertical(cv::Mat & InputArray, const std::vector<std::vector<cv::Point>> &model_contours, Control_Var& Ctrl_Var);			//双面纵向 不必要
CVDLL_API std::vector<std::vector<cv::Point>> CvGetDoubleContoursVertical(std::vector<std::vector<cv::Point>> model_contours);
CVDLL_API std::vector<std::vector<cv::Point>> CvGetDoubleContoursHorizental_Ex(std::vector<std::vector<cv::Point>> model_contours, cv::Mat * Out = nullptr);
CVDLL_API std::vector<std::vector<cv::Point>> CvGetDoubleContoursHorizental(std::vector<std::vector<cv::Point>> model_contours);
CVDLL_API void GetPointsContours(cv::Mat &InputArray, std::vector<cv::Point> &pts);


#endif
