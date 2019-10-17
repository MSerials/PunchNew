#pragma once
#include <iostream>
#include "opencv.hpp"
#include <opencv2\imgproc\types_c.h>
#include "global.h"
#include "../../MSerialsCamera/MSerialsCam.h"
#include "windows.h"


#ifndef CAMERAS_H
#define CAMERAS_H

#include "massert.h"

namespace MSerialsCamera {

enum{
    IMAGE_ORIGIN = 0, IMAGE_FLIPED = 1


};




static void addImageToBigImage(cv::Mat& BigImage, const cv::Mat &image, cv::Rect pos)
{
    try
    {
        cv::Mat imageROI = BigImage(pos);
        image.copyTo(imageROI);
    }
    catch (cv::Exception &e)
    {
        std::cout << "add pic error" << std::endl;
        throw e;

    }
}



    static int number = 0;

    static int init_camera(int ResSel = 2)
    {
        int c = enum_cameras();
        if (c > 0) {
            int  count_res = GetResolutionCount(0, 0);
            int MaxResSel = 5 < count_res ? 5 : count_res;
            ResSel = ResSel < MaxResSel?ResSel:MaxResSel;
            int width = 1024, height = 768;
            width = 2048;
            height  = 1536;
            if(2==ResSel)
                ResSel = 1;
            SetResolution(width, height, ResSel, 0, 0);
            std::cout << "sel is "<< ResSel <<" ccount res:" << count_res << " w:" << width << " h:" << height << std::endl;
        }
        number = c;
        return c;
    }

    static int camera_number()
    {
        return number;
    }



    static void CvtColor(const cv::Mat org, cv::Mat & dst , int sel = CV_BGR2GRAY )
    {
        CV_Assert(!org.empty());
        switch (sel)
        {
        case CV_BGR2GRAY:
            if (CV_8UC1 == org.type())
            {
                dst = org.clone();
            }
            else
            {
                cv::cvtColor(org, dst, CV_BGR2GRAY);
            }
            break;
        case CV_GRAY2BGR:
            if (CV_8UC3 == org.type())
            {
                dst = org.clone();
            }
            else
            {
                cv::cvtColor(org, dst, CV_GRAY2BGR);
            }
        }
    }

    //static cv::Mat TmpImage = cv::Mat(IMAGE_HEIGHT,IMAGE_WIDTH,CV_8UC1,cv::Scalar(250,20,20));

    static cv::Mat & VoidImage()
    {
        return _global::GetIns()->Image;
    }

    static bool isEqual(cv::Mat &a, cv::Mat &b)
    {
        int aSize = a.cols*a.rows;
        int bSize = b.cols*b.rows;
        if (aSize != bSize)
            return false;
        int len = aSize < bSize ? aSize : bSize;
        uchar *aData = a.data, *bData = b.data;
        for (int i = 0; i < len; i++)
        {
            if (*aData != *bData)
                return false;
            aData++;
            bData++;
        }
        return true;
    }





    static cv::Mat & GetMachineImage(int sel, double cam_angle =0.0,int Delay = 0, cv::Mat m_cameraMatrix = cv::Mat(), cv::Mat m_distCoeffs = cv::Mat()) {

        //snap已经包含了互斥锁，所以这个函数是阻塞的
        using namespace cv;
        static Mat Camera_Image, Camera_Snap,Camera_Snap_Origin;
        int width = 0, height = 0, ch = 3;
        uchar *data = nullptr;

        try{
        Snap(width, height, &data, ch, 0, 0, Delay);
        }
        catch(std::out_of_range ex){
            cv::resize(Camera_Snap_Origin, VoidImage(), cv::Size(IMAGE_WIDTH,IMAGE_HEIGHT), 0.0,0.0, cv::INTER_CUBIC);
            return VoidImage();
        }
        catch(std::exception &ex){
            cv::resize(Camera_Snap_Origin, VoidImage(), cv::Size(IMAGE_WIDTH,IMAGE_HEIGHT), 0.0,0.0, cv::INTER_CUBIC);
            return VoidImage();
        }
        if (nullptr == data)
        {
            static cv::Mat VoidImage_Ex;
            cv::Mat Rot_Mat = cv::getRotationMatrix2D(cv::Point2f(VoidImage().cols / 2.0, VoidImage().rows / 2.0), cam_angle, 1.0);
            cv::warpAffine(VoidImage(), VoidImage_Ex, Rot_Mat, VoidImage().size());
            return VoidImage_Ex;
        }
        //这个重新赋值w和h的目的是实时矫正
        if (width != Camera_Snap_Origin.cols || height != Camera_Snap_Origin.rows)
        {
            if (3 == ch) Camera_Snap_Origin = cv::Mat(cv::Size(width, height), CV_8UC3);
            else Camera_Snap_Origin = cv::Mat(cv::Size(width, height), CV_8UC1);
        }
        Camera_Snap_Origin.data = data;
        cv::resize(Camera_Snap_Origin, Camera_Image, cv::Size(IMAGE_WIDTH,IMAGE_HEIGHT), 0.0,0.0, cv::INTER_CUBIC);
        if (IMAGE_ORIGIN == sel)
        {
            static cv::Mat Gray;
            CvtColor(Camera_Image,Gray);
            return Gray;
        }

            if (!m_cameraMatrix.empty() && !m_distCoeffs.empty()) {
                cv::Size imageSize;
                imageSize.width = Camera_Image.cols;
                imageSize.height = Camera_Image.rows;
                Size newImageSize = imageSize;
                initUndistortRectifyMap(m_cameraMatrix, m_distCoeffs, Mat(),
                    getOptimalNewCameraMatrix(m_cameraMatrix, m_distCoeffs, imageSize, 1, newImageSize, 0), newImageSize, CV_16SC2, _global::GetIns()->map1, _global::GetIns()->map2);
                _global::GetIns()->ToRemap = true;
                std::cout << "可以映射了" << std::endl;
            }

        static cv::Mat rview;
        if (_global::GetIns()->ToRemap)
        {
#ifdef NO_MOTION
             printf("矫正图片\n");
#endif
            remap(Camera_Image, rview, _global::GetIns()->map1, _global::GetIns()->map2, INTER_LINEAR);
        }
        else
        {
#ifdef NO_MOTION
             printf("没有矫正图片\n");
#endif
            rview = Camera_Image.clone();
        }
       // int len = rview.cols>rview.rows?rview.cols:rview.rows;
        cv::Mat Rot_Mat = cv::getRotationMatrix2D(cv::Point2f(rview.cols / 2.0, rview.rows / 2.0), cam_angle, 1.0);
        cv::warpAffine(rview, Camera_Snap, Rot_Mat, rview.size());
        static cv::Mat Gray;
        CvtColor(Camera_Snap, Gray, CV_BGR2GRAY);
        return Gray;
    }


    //检测图像完整性，因为偶尔会有图像采集不到，会黑色
    static bool isImageOK(cv::Mat & InputArray){
            int ThresholdBlackDots = 2*InputArray.cols,BlackDots = 0;
            uchar *data = InputArray.data;
            for( int h = 0; h< InputArray.rows; h++){
                for(int w = 0;w<InputArray.cols;w++){
                    if(0x0 == data[w]){
                        BlackDots++;
                    }
                }
                data += InputArray.step;
            }
            if(BlackDots > ThresholdBlackDots){
                return false;
            }
            return true;
    }
    //拍摄两次用于检测相机是否有问题
    static void SnapTwice(cv::Mat &Snap, int Delay = 30)
    {

                //#define DELAY_SNAP 50
                clock_t clk = clock();
                cv::Mat Image_Try   = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
                clock_t endClk = clock();
                if((endClk - clk) < Delay)
                {
                    Sleep(Delay - (endClk - clk));
                }
                Snap               = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();

                if(true == MSerialsCamera::isEqual(Snap,Image_Try)
                   ||false == isImageOK(Snap)
                        )
                {
                    for (int i = 0;;i++)
                    {
                        Sleep(100);
                        if(MSerialsCamera::init_camera() > 0)
                        {
                            Snap      = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED).clone();
                            if(isImageOK(Snap))
                                    break;
                        }
                        if (i>30)
                        {
                            throw std::exception("相机重连失败!，是否相机掉线？");
                        }
                }
           }

    }//EndSnapTwice



};

#endif // CAMERAS_H
