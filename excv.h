#ifndef EXCV_H
#define EXCV_H
#include "opencv.hpp"
#include <opencv2\imgproc\types_c.h>
#include "HalconCpp.h"
#include <vector>
#include <QDateTime>
#include "preferences.h"


namespace Excv {

#ifndef AN_HOUR
#define AN_HOUR	3600000
#endif

template<typename _T>
static bool WaitValue(_T &T,size_t TimeOut = 2000)
{
    clock_t clk = clock();
    while(T>0)
    {
        if((clock()-clk)>TimeOut)
        {
            return false;
        }
        Sleep(2);
    }
    return true;
}

//六个函数分别表示像素和距离的转化
static inline void pix_to_mm(const cv::Point &pix, cv::Point2d &mm) {
    mm.x = pix.x*X_RATIO;
    mm.y = pix.y*Y_RATIO;
}

static inline void pix_to_pls(const cv::Point &pix, cv::Point &pls) {
    if (X_DIS_PULS_RATIO < 0.0000001 || Y_DIS_PULS_RATIO < 0.0000001)
    {
        pls.x = 0;
        pls.y = 0;
        return;
    }
    pls.x = pix.x * X_RATIO / X_DIS_PULS_RATIO;
    pls.y = pix.y * Y_RATIO / Y_DIS_PULS_RATIO;
}

static inline void mm_to_pls(const cv::Point2d &mm, cv::Point &pls) {
    if (X_DIS_PULS_RATIO < 0.0000001 || Y_DIS_PULS_RATIO < 0.0000001)
    {
        pls.x = 0;
        pls.y = 0;
        return;
    }
    pls.x = mm.x / X_DIS_PULS_RATIO;
    pls.y = mm.y / Y_DIS_PULS_RATIO;

}

static inline void mm_to_pix(const cv::Point2d &mm, cv::Point &pix) {
    if (X_RATIO < 0.0000001 || Y_RATIO < 0.0000001)
    {
        pix.x = 0;
        pix.y = 0;
        return;
    }
    pix.x = static_cast<int>(mm.x / X_RATIO);
    pix.y = static_cast<int>(mm.y / Y_RATIO);
}

static inline void pls_to_pix_y(const int pls, int &pix)
{
    if (Y_RATIO < 0.0000001)
    {
        pix = 0;
        return;
    }
    pix = pls * Y_DIS_PULS_RATIO / Y_RATIO;
}

static inline void pls_to_pix(const cv::Point &pls, cv::Point &pix) {
    if (X_RATIO < 0.0000001 || Y_RATIO < 0.0000001)
    {
        pix.x = 0;
        pix.y = 0;
        return;
    }
    pix.x = pls.x * X_DIS_PULS_RATIO / X_RATIO;
    pix.y = pls.y * Y_DIS_PULS_RATIO / Y_RATIO;
}

static inline void pls_to_mm(const cv::Point &pls, cv::Point2d &mm) {
    mm.x = pls.x * X_DIS_PULS_RATIO;
    mm.y = pls.y * Y_DIS_PULS_RATIO;
}

static inline void AxisYpls2pix(int pls, int & pix){
    CV_Assert(Y_RATIO > 0.0000001);
    pix = pls * Y_DIS_PULS_RATIO / Y_RATIO;
}


static inline void Contours_Move(const std::vector<std::vector<cv::Point>> &origin, std::vector<std::vector<cv::Point>> &dst, cv::Point direct)
{

    dst.assign(origin.begin(), origin.end());
    for (size_t i = 0; i < origin.size(); i++)
        for (size_t p = 0; p < origin[i].size(); p++)
        {
            dst[i][p].x = origin[i][p].x + direct.x;
            dst[i][p].y = origin[i][p].y + direct.y;
        }
}


static bool CheckCabliImage(const cv::Mat & InputArray,cv::Mat &color_view, cv::Size board_size = cv::Size(6,9))
{
    using namespace cv;
    std::vector<cv::Point2f> image_points_buf;  /* 缓存每幅图像上检测到的角点 */
    if (0 == findChessboardCorners(InputArray, board_size, image_points_buf)) {
        return false;
    }
    find4QuadCornerSubpix(InputArray, image_points_buf, Size(11, 11)); //对粗提取的角点进行精确
    cv::drawChessboardCorners(color_view, board_size, image_points_buf, true); //用于在图片中标记角点
    return true;
}

//保存图片
static std::string cv_write_image(const cv::Mat & InputArray, const char* path = "", const char *file_name = "", bool AddDateTime = false)
{


    QString DateTime = QDateTime::currentDateTime().toString("yyyy_MM_dd-hh_mm_ss");
    std::string fileName = "";
    if(AddDateTime)
        fileName = std::string(path) + "/" + std::string(file_name) + "_" + DateTime.toStdString();
    else
        fileName = std::string(path) + "/" + std::string(file_name);
     cv::imwrite(fileName+".png", InputArray);
    return fileName+".png";

#ifdef __AFX_H__

    CString Path(path), FileName(file_name), Time_zzz(_Timer::GetIns()->Get_Time_zzz()),fileName;
    if (0 == strlen(path) && 0 == strlen(file_name))
        fileName = Time_zzz;
    else if (0 == strlen(file_name))
        fileName = Path + "/" + Time_zzz;
    else
        fileName = Path + "/" + FileName + "_" + Time_zzz;
    cv::imwrite((cv::String)(CStringA)(fileName+L".bmp"), m);
    return (std::string)(CStringA)(fileName + L".bmp");
#endif
    return "\0";
}




static void MatToHObj(const cv::Mat & image, HalconCpp::HObject &Hobj)
{
    CV_Assert(!image.empty());
    using namespace cv;
    using namespace HalconCpp;

    int hgt=image.rows;
    int wid=image.cols;
    int i;
    //	CV_8UC3
    if(image.type() == CV_8UC3)
    {
        std::vector<cv::Mat> imgchannel;
        split(image,imgchannel);
        Mat imgB=imgchannel[0];
        Mat imgG=imgchannel[1];
        Mat imgR=imgchannel[2];
        uchar* dataR=new uchar[hgt*wid];
        uchar* dataG=new uchar[hgt*wid];
        uchar* dataB=new uchar[hgt*wid];
        for(i=0;i<hgt;i++)
        {
            memcpy(dataR+wid*i,imgR.data+imgR.step*i,wid);
            memcpy(dataG+wid*i,imgG.data+imgG.step*i,wid);
            memcpy(dataB+wid*i,imgB.data+imgB.step*i,wid);
        }
        GenImage3(&Hobj,"byte",wid,hgt,(Hlong)dataR,(Hlong)dataG,(Hlong)dataB);
        delete []dataR;
        delete []dataG;
        delete []dataB;
        dataR=NULL;
        dataG=NULL;
        dataB=NULL;
    }
    //	CV_8UCU1
    else if(image.type() == CV_8UC1)
    {
        uchar* data=new uchar[hgt*wid];
        for(i=0;i<hgt;i++)
            memcpy(data+wid*i,image.data+image.step*i,wid);
        GenImage1(&Hobj,"byte",wid,hgt,(Hlong)data);
        delete[] data;
        data=NULL;
    }
}

static void h_disp_obj(HalconCpp::HObject &obj, HalconCpp::HTuple &disp_hd) {
    using namespace HalconCpp;
    try
    {
        HTuple w, h;
        GetImagePointer1(obj, NULL, NULL, &w, &h);
        HDevWindowStack::Push(disp_hd);
        HDevWindowStack::SetActive(disp_hd);
        SetPart(disp_hd, NULL, NULL, h, w);
        DispObj(obj, disp_hd);
    }
    catch (HException &except)
    {
        HTuple ExceptionMessage;
        except.ToHTuple(&ExceptionMessage);
        SetTposition(disp_hd, 0, 1);
        WriteString(disp_hd, ExceptionMessage);
    }

}

}

#endif // EXCV_H
