#ifndef MDXF_H
#define MDXF_H

#include <list>
#include <vector>
#include "opencv.hpp"
#include "HalconCpp.h"
#include "debuginfo.h"
using namespace HalconCpp;



#define ho_ObjectSelected MDxf_Var::GetIns()->m_ho_ObjectSelected
#define ho_Boundaries MDxf_Var::GetIns()->m_ho_Boundaries

namespace MSerials
{
    class MDxf_Var {
    private:
        MDxf_Var() { printf("MDxf_Var\n");
                   GenEmptyObj(&m_ho_Boundaries);
                   }

    //双锁保证线程安全
    public:
        static MDxf_Var * GetIns()
        {
            static  MDxf_Var _g;
            return &_g;
            /**
            static MDxf_Var *Ins = nullptr;
            static std::mutex mtx;
            if (nullptr == Ins)
            {
                static lock_guard<std::mutex> lck(mtx);
                if (nullptr == Ins) {
                    Ins = new MDxf_Var();
                }
            }
            //虽然没用
            if (nullptr == Ins)
            {
                printf("");
                return nullptr;
            }
            return Ins;
            */
        }
        HObject m_ho_ObjectSelected, m_ho_Boundaries;
    };



    class MDxf
    {
    private:
        // Local iconic variables
        HObject  ho_Models, ho_Contours, ho_EmptyObject;
        HObject  ho_ContoursClassified, ho_Holes;
        HObject  ho_SortedBoundaries, ho_Image;
        HObject  ho_Domain, ho_ModelImages, ho_ModelRegions, ho_ConnectedRegions;
        HObject  ho_SelectedRegions, ho_SObj, ho_ModelContours, ho_ModelCont;
        HObject  ho_ModelSelected, ho_ModelTrans;

        // Local control variables
        HTuple  hv_Error;
        HTuple  hv_MinW=1, hv_MaxW=100, hv_MinH=1, hv_MaxH=100, hv_ContoursHeight;
        HTuple  hv_WindowHandle, hv_Colors, hv_IndexS, hv_IndexE;
        HTuple  hv_ModelIDs, hv_DxfStatus, hv_Number, hv_i, hv_R1;
        HTuple  hv_C1, hv_R2, hv_C2, hv_RectW, hv_RectH, hv_origin_x;
        HTuple  hv_origin_y, hv_isFirstPoint, hv_Area, hv_Row, hv_Column;
        HTuple  hv_NumModels, hv_J, hv_ModelID, hv_NumModel, hv_Row1;
        HTuple  hv_Column1, hv_Button1, hv_Button, hv_ImgNo, hv_S1;
        HTuple  hv_Angle, hv_Score, hv_Model, hv_S2, hv_Time, hv_Num;
        HTuple  hv_HomMat2D, hv_R, hv_C;
        //参数对齐项
        HTuple  hv_StartX = 2147483647;
        HTuple  hv_StartY = 2147483647;


    public:
        MDxf() { SetCheck("~give_error"); }

        HObject& ReadDxf(const char* fileName, int64 winHandle = 0)
        {
            try {
                ReadContourXldDxf(&ho_Contours, fileName, HTuple(), HTuple(), &hv_DxfStatus);
                return ReLoadPara(winHandle);
            }
            catch (HException error)
            {
                HTuple Error;
                error.ToHTuple(&Error);
                std::string e(Error.ToString().Text());
                throw MException(e.c_str());
            }
            catch (MException &e)
            {
                throw e;
            }
        }

        void SetPara(double Minh = 1, double Maxh =100, double Minw = 1, double Maxw =100)
        {
            hv_MinW = Minw;
            hv_MinH = Minh;
            hv_MaxW = Maxw;
            hv_MinW = Minw;
        }

        std::list<std::vector<cv::Point2d>> GetDxfPoints(int64 hv_WindowHandle = 0)
        {
            std::list<std::vector<cv::Point2d>> points;
            std::vector<cv::Point2d> linePoints;
            double RStart = 0.0, OldRStart = 0.0;
            HTuple hv_isFirstPoint = 0;
            try{
                SetCheck("~give_error");
                CountObj(ho_Boundaries, &hv_Number);
                if(hv_Number<1){
                    printf("异常在GetDxfPoints");
                    throw MException("Dxf里没有找到点,或者没有读取到dxf文件");
                }
                {
                    HTuple end_val124 = hv_Number;
                    HTuple step_val124 = 1;
                    for (hv_i = 1; hv_i.Continue(end_val124, step_val124); hv_i += step_val124)
                    {
                        SelectObj(ho_Boundaries, &ho_ObjectSelected, hv_i);
                        SmallestRectangle1Xld(ho_ObjectSelected, &hv_R1, &hv_C1, &hv_R2, &hv_C2);
                        if (0 != (0 == hv_isFirstPoint))
                        {
                            hv_isFirstPoint = 1;
                            OldRStart = hv_R1[0].D();
                            RStart = OldRStart;
                            linePoints.push_back(cv::Point2d(hv_C1,hv_R1));
                            //结尾处理
                            if(hv_i == hv_Number)
                            {
                                points.push_back(linePoints);
                            }
                            if (0 != hv_WindowHandle)
                            {
                                SetColor(hv_WindowHandle, "red");
                                DispObj(ho_ObjectSelected, hv_WindowHandle);
                            }
                        }
                        else
                        {
                            RStart = hv_R1[0].D();
                            linePoints.push_back(cv::Point2d(hv_C1,hv_R1));
                            if(hv_i == hv_Number)
                            {
                                points.push_back(linePoints);
                            }
                            //10微米就转行
                            else if((OldRStart - RStart) > 0.01)
                            {
                                points.push_back(linePoints);
                                linePoints.clear();
                            }


                            OldRStart =RStart ;
                            if (0 != hv_WindowHandle)
                            {
                                SetColor(hv_WindowHandle, "cyan");
                                DispObj(ho_ObjectSelected, hv_WindowHandle);
                            }
                        }
                    }
                }
                if(points.empty())
                    {
                    printf("异常，dxf文件没有点\n");
                    throw MException("Dxf里没有找到点,或者没有读取到dxf文件");

                    }
                return points;
            }
            catch(HException &e)
            {
                HTuple ErrorInfo;
                e.ToHTuple(&ErrorInfo);
                printf(ErrorInfo.ToString().Text());
                throw MException("No Points");
            }
            catch(MException &e)
            {
                printf(e.what());
                throw MException("Dxf里没有找到点,或者没有读取到dxf文件");
            }

        }

        //获取轮廓里的点的位置，注意单位是mm
        HObject& ReLoadPara(int64 winHandle = 0)
        {
            try {
                hv_WindowHandle = winHandle;
                UnionAdjacentContoursXld(ho_Contours, &ho_Contours, 1, 1, "attr_keep");

                //第一次用于选取特征点，特征用矩形高宽表示
                GenEmptyObj(&ho_EmptyObject);
                DifferenceClosedContoursXld(ho_Contours, ho_EmptyObject, &ho_ContoursClassified
                );
                GenEmptyObj(&ho_Boundaries);
                GenEmptyObj(&ho_Holes);
                CountObj(ho_ContoursClassified, &hv_Number);
                {
                    HTuple end_val87 = hv_Number;
                    HTuple step_val87 = 1;
                    for (hv_i = 1; hv_i.Continue(end_val87, step_val87); hv_i += step_val87)
                    {
                        SelectObj(ho_ContoursClassified, &ho_ObjectSelected, hv_i);
                        SmallestRectangle1Xld(ho_ObjectSelected, &hv_R1, &hv_C1, &hv_R2, &hv_C2);
                        hv_RectW = hv_C2 - hv_C1;
                        hv_RectH = hv_R2 - hv_R1;
                        if (0 != (hv_ContoursHeight == 0))
                        {
                            hv_ContoursHeight = hv_RectH;
                        }
                        if (0 != (HTuple(HTuple(hv_RectW < hv_MaxW).TupleAnd(hv_RectW > hv_MinW)).TupleAnd(HTuple(hv_RectH > hv_MinH).TupleAnd(hv_RectH < hv_MaxH))))
                        {
                            if (0 != (hv_C1 < hv_StartX))
                            {
                                hv_StartX = hv_C1;
                            }

                            if (0 != (hv_R1 < hv_StartY))
                            {
                                hv_StartY = hv_R1;
                            }
                            ConcatObj(ho_Boundaries, ho_ObjectSelected, &ho_Boundaries);
                        }
                    }
                }
                //第二次，将图像进行排序对齐,这样就可以显示在界面的左上角了
                SortContoursXld(ho_Boundaries, &ho_SortedBoundaries, "lower_right", "true", "row");
                CountObj(ho_SortedBoundaries, &hv_Number);
                GenEmptyObj(&ho_Boundaries);

                {
                    HTuple end_val111 = hv_Number;
                    HTuple step_val111 = 1;
                    for (hv_i = 1; hv_i.Continue(end_val111, step_val111); hv_i += step_val111)
                    {
                        SelectObj(ho_SortedBoundaries, &ho_ObjectSelected, hv_i);
                        VectorAngleToRigid(0, 0, 0, -hv_StartY, -hv_StartX, 0, &hv_HomMat2D);
                        AffineTransContourXld(ho_ObjectSelected, &ho_ModelTrans, hv_HomMat2D);
                        ConcatObj(ho_Boundaries, ho_ModelTrans, &ho_Boundaries);
                    }
                }

                //第三次次对Bounsaries的图形进行排序并找到所有的左上角点,用左上角第一个图形为原点，或者选取
                hv_origin_x = 0;
                hv_origin_y = 0;
                hv_isFirstPoint = 0;
                if (0 != winHandle)
                {
                    SetDraw(hv_WindowHandle, "fill");
                }
                CountObj(ho_Boundaries, &hv_Number);
                {
                    HTuple end_val124 = hv_Number;
                    HTuple step_val124 = 1;
                    for (hv_i = 1; hv_i.Continue(end_val124, step_val124); hv_i += step_val124)
                    {
                        SelectObj(ho_Boundaries, &ho_ObjectSelected, hv_i);
                        SmallestRectangle1Xld(ho_ObjectSelected, &hv_R1, &hv_C1, &hv_R2, &hv_C2);
                        if (0 != (0 == hv_isFirstPoint))
                        {
                            hv_isFirstPoint = 1;
                            if (0 != winHandle)
                            {
                                SetColor(hv_WindowHandle, "red");
                                DispObj(ho_ObjectSelected, hv_WindowHandle);
                            }
                        }
                        else
                        {
                            if (0 != winHandle)
                            {
                                SetColor(hv_WindowHandle, "cyan");
                                DispObj(ho_ObjectSelected, hv_WindowHandle);
                            }
                        }
                    }
                }

                if (0 != winHandle)
                {
                    SetDraw(hv_WindowHandle, "margin");
                    SetColor(hv_WindowHandle, "green");
                //    DispObj(ho_Boundaries, hv_WindowHandle);
                }

#ifdef NO_MOTION
                std::list<std::vector<cv::Point2d>> a = GetDxfPoints(winHandle);
                int c = 0;
                for(auto pts:a)
                {
                    for(auto pt:pts)
                    {
                        c++;
                    }
                }
                printf("dxf含有点%d个\n",c);
#endif

                return ho_Boundaries;

            }
            catch (HException error)
            {
                HTuple Error;
                error.ToHTuple(&Error);
                throw MException(Error.ToString().Text());
            }
        }


    };
}










#endif // MDXF_H
