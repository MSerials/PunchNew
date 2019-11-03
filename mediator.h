#pragma once
#include "HalconCpp.h"
#include "process.h"
#include "excv.h"
#include "opencv2/core.hpp"
#include "global.h"
#include "mdxf.h"
using namespace HalconCpp;
//变形的中介者模式，协调



//改进问题
//1 判断相机采图完整程度，如果有黑线，说明异常
//2 存储路径

#ifndef __MEMENTO__
#define __MEMENTO__

class Memento
{
    //现实中无法进行多个存档
    Control_Var Ctrl_Var;
public:
    Memento(){printf("Memento\n");}
    void Save(Control_Var pCtrl_Var){ Ctrl_Var = pCtrl_Var;}
    void Load(Control_Var &pCtrl_Var){ pCtrl_Var = Ctrl_Var;}
};

#endif


#ifndef MEDIATOR_H
#define MEDIATOR_H

class Mediator
{
public:
    bool AddOnePunch = false;
    std::string _program_name;
    Mediator(){Init();printf("init\n");}
    //相机拍摄完毕 //图像处理完毕
    bool isSnapOver = false,isDealOver = false;
    //图像如果查到有，就表示下次可以前进
    long long MachineState, CanBeMoveForwardPuls = -1;//= (STOP|NOT_ORIGIN);
    //相机缓存图片，计算使用的图片，显示使用的图片,针对大的模具进行图片扩增,另一个是计算用的,要做备份，否则再drawPts后丢失原有的信息
    cv::Mat Image,Cal_Image,Show_Image,Virtual_Image, Virtual_Image_Cal,m_cameraMatrix, m_distCoeffs;
public:
    MEvent evt_Process,evt_GetPoint,evt_Punch,evt_Memento;
    Memento memento;
    Control_Var Ctrl_Var;
    std::list<std::vector<cv::Point2d>> DxfPoints;
    int YNeedMovePuls = -1;                                                                 ///这个参数暂时不写入Control_Var，是为了猜测下一次距离冲，增加冲压速度
    MSerials::MDxf mdxf;
    HalconCpp::HObject HalconImage,DxfContours;
    HTuple MainWindowDispHd,SetDispHd,ModelDisp,NormalDisp;                                 ///三个界面图像指针
    typedef void (__stdcall *MsgBox)(std::string);
    typedef void (__stdcall *PushButton_)(void);
    MsgBox UpdateHistory = nullptr;
    void UpdateMessage(std::string msg){if(nullptr != UpdateHistory) UpdateHistory(msg);}   ///消息通知主界面
    PushButton_ show_StartButton = nullptr;                                                 ///call back main window button
    PushButton_ show_Counter     = nullptr;                                                 ///显示冲压了多少次

    void AddPointUnderDoubleModleAndFill(Control_Var & Ctrl_Var);
    //在扩充模式下合并上一次和现在的图片
    inline void mergeImage(const cv::Mat & PreviousImage, const cv::Mat & CurrentImage, cv::Mat &Dst, cv::Mat &Show_Image, Control_Var &Ctrl_Var);

    static Mediator * GetIns(){static Mediator m_me; return &m_me;}

    static unsigned int __stdcall InitCameraAndMotionCard(void* pLVOID){
        static std::mutex m_mtx;std::lock_guard<std::mutex> lck(m_mtx);std::string InitInfo;
        Mediator::GetIns()->MachineState |= (NO_CAMERA |NO_MOTION_CARD);
        int Camera_qty = MSerialsCamera::init_camera(MODE_INDEX);
        if(Camera_qty < 1){InitInfo += "没有发现相机...";}
        else{
            Mediator::GetIns()->MachineState &= ~NO_CAMERA;
            set_gamma(CAMERAGAMMA);
        }

        if(左方向为原点){ERRORLOG("左方向为原点，设置错误会导致运行出问题")}
        else{ERRORLOG("右方向为原点，设置错误会导致运行出问题")}

        //    Mediator::GetIns()->UpdateMessage(QString::number(X_AXIS_LIMIT).toLocal8Bit().data());
        int Card_Qty = motion::GetIns()->init(左方向为原点);
        if(Card_Qty < 1) {
            InitInfo += "没有发现控制卡...";
        }
        else{
            if(左方向为原点)
            {
                if(X_AXIS_LIMIT < 1000)  X_AXIS_LIMIT = 1000;
            }
            else
            {
                if(X_AXIS_LIMIT > -1000)  X_AXIS_LIMIT = -1000;
            }
            motion::GetIns()->CurrentCard()->SetLimit(X_AXIS_MOTOR, X_AXIS_LIMIT,左方向为原点);
            Mediator::GetIns()->MachineState &= ~NO_MOTION_CARD;
        }

        if(!InitInfo.empty())
        {
            ERRORLOG(InitInfo);
        }
        else
        {
            ERRORLOG("初始化完毕");
        }
        return 0;
    }

private:
    //初始化
    void Init(){
        MachineState = (STOP|NOT_ORIGIN|NO_CAMERA |NO_MOTION_CARD);
        _beginthreadex(NULL, 0,InitCameraAndMotionCard, this, 0, NULL);
        HANDLE thread_punch = (HANDLE)_beginthreadex(NULL, 0,GetPoints, this, 0, NULL);
        thread_punch = (HANDLE)_beginthreadex(NULL, 0,MovePunch, this, 0, NULL);
        SetThreadPriority(thread_punch,REALTIME_PRIORITY_CLASS);
        _beginthreadex(NULL, 0,IOScanner, this, 0, NULL);(HANDLE)_beginthreadex(NULL, 0,Process_, this, 0, NULL);(HANDLE)_beginthreadex(NULL, 0,thread_Memento, this, 0, NULL);
    }

    //相机处理
    static unsigned int __stdcall GetPoints(void* pLVOID){pLVOID  = nullptr;
                                                          for(;;){
                                                              ::WaitForSingleObject(Mediator::GetIns()->evt_GetPoint.get(),INFINITE);
                                                              Mediator::GetIns()->Get_Points(Mediator::GetIns()->Ctrl_Var.Cal_ModelsPostion);
                                                              Mediator::GetIns()->evt_GetPoint.ResetEvent();
                                                          }
                                                         }

    //冲压控制
    static unsigned int __stdcall MovePunch(void* pLVOID){pLVOID  = nullptr;
                                                          for(;;){
                                                              ::WaitForSingleObject(Mediator::GetIns()->evt_Punch.get(),INFINITE);
                                                              std::string Res = Mediator::GetIns()->Move_Punch();
                                                              if(0 != strcmp("OK",Res.c_str())) { ERRORLOG(Res);Mediator::GetIns()->MachineOp(MACHINE_STOP); }
                                                              Mediator::GetIns()->evt_Punch.ResetEvent();
                                                          }
                                                         }

    //控制过程选择，是视觉还是数控
    static unsigned int __stdcall Process_(void* pLVOID);

    static unsigned int __stdcall thread_Memento(void *pLVOID){pLVOID = nullptr;
                                                               for(;;){
                                                                   ::WaitForSingleObject(Mediator::GetIns()->evt_Memento.get(),INFINITE);
                                                                   clock_t clk = clock();
                                                                   while(Mediator::GetIns()->evt_GetPoint.State() || Mediator::GetIns()->evt_Punch.State() || Mediator::GetIns()->evt_Process.State()){
                                                                       if((clock()-clk)>10000){ ERRORLOG("存档超时"); Mediator::GetIns()->MachineOp(MACHINE_STOP);break;}
                                                                       Sleep(5);
                                                                   }
                                                                   Mediator::GetIns()->memento.Save(Mediator::GetIns()->Ctrl_Var);
                                                                   Mediator::GetIns()->evt_Memento.ResetEvent();
                                                               }
                                                              }

    static unsigned int __stdcall IOScanner(void* pLVOID){pLVOID  = nullptr;
                                                          for(;;){
                                                              Sleep(20);
                                                              Mediator::GetIns()->IO_Scanner();
                                                          }
                                                         }

public:
    //唯一能知道的暂停状态是前进的Y轴和 目前的
    long long GetState(){return MachineState;}
    //从控制系统的脉冲变成像素
    void PulsPos2PixPos(const std::list<std::vector<cv::Point2l>> & PulsPos, std::list<std::vector<cv::Point>> & PixPos, Control_Var Ctrl_Var);
    void PixPos2PulsPos(const std::list<std::vector<cv::Point2l>> & PixPos, std::list<std::vector<cv::Point>> & PulsPos, Control_Var Ctrl_Var);
    void PrepareCalImageAndShowImage(cv::Mat & InputArray,cv::Mat & Cal_Image, cv::Mat & Show_Image, Control_Var & Ctrl_Var);
    std::list<std::vector<cv::Point>> Get_Points_Image(cv::Mat & InputArray, const std::vector<std::vector<cv::Point>> ModelContours, Control_Var Ctrl_Var, int Sel)
    {
#ifdef NO_MOTION
        clock_t clk = clock();
#endif
        try {
            Ctrl_Var.Lines_Method= Sel;
            std::vector<std::vector<cv::Point>>  Contours= ModelContours;
            std::list<std::vector<cv::Point>> ImagePoints;
            PrepareCalImageAndShowImage(InputArray,Cal_Image,Show_Image,Ctrl_Var);

            switch (Sel)
            {
            case LINES_HORIZONTAL:
                ImagePoints =  CvGeAllPointsHorizentalEx(Cal_Image, Contours, Ctrl_Var);
                break;
            case LINES_VERTICAL:
                ImagePoints =  CvGeAllPointsVerticalEx(Cal_Image, Contours, Ctrl_Var);
                break;
            case LINES_HORIZONTAL_FILL:
                mergeImage(InputArray,Cal_Image,Virtual_Image_Cal,Show_Image,Ctrl_Var);
                ImagePoints = CvGeAllPointsHorizentalAIFillEx(Virtual_Image_Cal, Contours, Ctrl_Var);
                break;
            case LINES_HORIZONTAL_USER:
                mergeImage(InputArray,Cal_Image,Virtual_Image_Cal,Show_Image,Ctrl_Var);
                ImagePoints = CvGeAllPointsHorizentalAIFillSingle(Virtual_Image_Cal, Contours, Ctrl_Var);
                break;
            default:
                ImagePoints =  CvGeAllPointsHorizentalEx(Cal_Image, Contours, Ctrl_Var);
                break;
            }
#ifdef NO_MOTION
            char Error[256] = {0};
            sprintf(Error,"遍历时间 %d ms",clock()-clk);
            ERRORLOG(Error);
#endif
            return ImagePoints;
        }
        catch (cv::Exception e)
        {
            char Error[256] = {0};
            if(strlen(e.what()) > 128) throw e;
            sprintf(Error,"GetPoints Image Process Error %s",e.what());
            printf_s("%s",Error);
            throw std::exception(Error);
        }
        catch(std::out_of_range e)
        {
            char Error[256] = {0};
            if(strlen(e.what()) > 128) throw e;
            sprintf(Error,"GetPoints Image Process Error %s",e.what());
            throw std::exception(Error);

        }


    }

    void AddCalPosToModelsPos(std::list<std::vector<cv::Point>> & CalPos, std::list<std::vector<cv::Point2l>> & ModelsPos);
    void IOC();
    void DigitalProcess();                                          /////数控模式
    void Process();                                                 /////视觉模式
    bool Get_Points(std::list<std::vector<cv::Point>> &Points);     ///获取图像中的点集

    bool CheckPunchTimeOut()
    {
        //return true;
        for(int i = 0;i< 8;i++) motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, ON);
        bool porigin_state = (IN_PUNCH_ORIGIN == (IN_PUNCH_ORIGIN & motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_IO_SEL)));
        bool poldorigin_state = porigin_state;
        bool isDownStair = false;
        clock_t clk_move_record = clock();
        for (;;)
        {
            Sleep(0);
            clock_t current_clock = clock();
            porigin_state = (IN_PUNCH_ORIGIN == (IN_PUNCH_ORIGIN & motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_IO_SEL)));
            if (poldorigin_state != porigin_state)
            {
                clk_move_record = current_clock;
                //if 下降沿 else 上升沿
                if (porigin_state)      isDownStair = true;
                else                    isDownStair = false;
            }
            poldorigin_state = porigin_state;

            if (isDownStair && ((current_clock - clk_move_record) > TIME_GAP))
            {
                for(int i = 0;i< 8;i++) motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, OFF);
                return true;
            }


#if 0
            if ((current_clock - clk_move_record) > 500)
            {
                for(int i = 0;i< 8;i++) motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, OFF);
                //  motion::GetIns()->CurrentCard()->e_stop(-1);
                return true;
            }
#endif

            if ((current_clock - clk_move_record) > 6000)
            {
                for(int i = 0;i< 8;i++) motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, OFF);
                motion::GetIns()->CurrentCard()->e_stop(-1);
                return false;
            }
        }
        UpdateMessage("异常错误 InCheckPunchTimeOut");
        return true;
    }

    void SortPunchVector(std::vector<cv::Point2l>& Vec)
    {
        try {
            long lx_puls = X_CAM_DISTANCE_PLS - static_cast<long>(Vec.at(0).x);
            long rx_puls = X_CAM_DISTANCE_PLS - static_cast<long>(Vec.at(Vec.size() - 1).x);
            long cpos = static_cast<long>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS));
            if (abs(cpos - lx_puls) > abs(cpos - rx_puls))
            {
                std::sort(Vec.begin(), Vec.end(), [](cv::Point a, cv::Point b) { return (a.x > b.x); });
            }
        }
        catch (std::out_of_range e)
        {
            throw e;
        }
    }

    std::string MoveBeforePunch(size_t &ModelsPostionSize, size_t &Old_ModelsPostionSize,  Control_Var& Ctrl_Var, clock_t &clk_move_record)
    {
        try{
            static long long lastCommand = MachineState;
            if ((ModelsPostionSize != Old_ModelsPostionSize) && (ModelsPostionSize > 0) && (Old_ModelsPostionSize < 1))
            {
                CV_Assert(!Ctrl_Var.ModelsPostion.empty());
                long x_move_puls = X_CAM_DISTANCE_PLS - Ctrl_Var.ModelsPostion.begin()->at(0).x;
                motion::GetIns()->CurrentCard()->absolute_move(X_AXIS_MOTOR, x_move_puls, 50, X_AXIS_SPEED,2);
                clk_move_record = clock();
                for (; x_move_puls != motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS, X_AXIS_MOTOR);) {
                    Sleep(5);
                    lastCommand = MachineState;
                    if(PAUSE == (PAUSE&lastCommand))
                    {
                        Old_ModelsPostionSize = ModelsPostionSize;
                        return "OK";
                    }else if(STOP == (STOP&lastCommand))
                    {
                        return "按下了停止";
                    }
                    if((clock()-clk_move_record)>10000)
                    {
                        return "十秒内没到指定位置，可能：1.X轴报警，2.X轴限位太右侧，3.移动太慢";
                    }
                }
                Ctrl_Var.CurrentXAxisPostion = static_cast<long>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS));
            }
            else if((PAUSE == (PAUSE&lastCommand)) && (ModelsPostionSize > 0))
            {
                CV_Assert(!Ctrl_Var.ModelsPostion.empty());
                long x_move_puls = X_CAM_DISTANCE_PLS - Ctrl_Var.ModelsPostion.begin()->at(0).x;
                motion::GetIns()->CurrentCard()->absolute_move(X_AXIS_MOTOR, x_move_puls, 50, X_AXIS_SPEED,20);
                clk_move_record = clock();
                for (; x_move_puls != motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS, X_AXIS_MOTOR);) {
                    Sleep(5);
                    lastCommand = MachineState;
                    if(PAUSE == (PAUSE&lastCommand))
                    {
                        Old_ModelsPostionSize = ModelsPostionSize;
                        return "OK";
                    }else if(STOP == (STOP&lastCommand))
                    {
                        return "按下了停止";
                    }
                    if((clock()-clk_move_record)>10000)
                    {
                        return "十秒内没到指定位置，可能：1.X轴报警，2.X轴限位太右侧，3.移动太慢";
                    }
                }
                Ctrl_Var.CurrentXAxisPostion = static_cast<long>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS));
            }
            //按下暂停，然后停止的状态会导致误检
            Old_ModelsPostionSize = ModelsPostionSize;
            return "OK";
        }catch(cv::Exception Ex)
        {
            throw Ex;
        }
    }


    //需要优化
    std::string PrepareMovePunchParameter(Control_Var & Ctrl_Var)
    {
        Ctrl_Var.MovingForwardPuls = 0;
        Ctrl_Var.dMovingForwardPuls = 0;
        std::vector<std::vector<cv::Point>> Contours;
        cv::Rect rt;
        double rtHegiht, rtROI;
        CV_Assert(!Ctrl_Var.ModelContours.empty());
        if (X_DIS_PULS_RATIO < 0.0000001 || X_DIS_PULS_RATIO < 0.0000001 || X_RATIO < 0.0000001 || Y_RATIO < 0.0000001){
            return "参数错误，X轴或者Y轴距离脉冲比太，或者像素转卡尺尺寸参数不对";
        }

        rtROI = CHECK_R2 - CHECK_R1;
        rt = cv::boundingRect(Ctrl_Var.ModelContours.at(0));
        rtHegiht = static_cast<double>(rt.height);
        switch (LINE_METHOD)
        {
        case LINES_VERTICAL:
            rtHegiht = static_cast<double>(1.5*rt.height);
            break;
        }
        //考虑的。。。
        //考虑到三种情况实际，一种是没物料，一种是物料了，还一种是没物料的时候 错误的认为脉冲太小
        //  std::string guocheng;


        if(!(LINES_HORIZONTAL_FILL == LINE_METHOD
             || LINES_HORIZONTAL_USER == LINE_METHOD
             ))
        {
            if (rtROI < ROI_MODEL_RATIO * rtHegiht)
            {
                //检测区域减高度差
                double Height_C = rtROI - rtHegiht;
                Ctrl_Var.dMovingForwardPuls = -Y_RATIO * Height_C / Y_DIS_PULS_RATIO;
                Ctrl_Var.MovingForwardPuls = static_cast<long>(Ctrl_Var.dMovingForwardPuls);

                if(Ctrl_Var.DealMethod_Ctrl == tagDealMethod::FINDHEADPIX){
                    double Puls =    -Y_RATIO * Ctrl_Var.Header_Pix_Ctrl / Y_DIS_PULS_RATIO;
                    if((Puls < 0) && abs(Puls) > abs(Ctrl_Var.dMovingForwardPuls) )
                    {
                        Ctrl_Var.dMovingForwardPuls = Puls;
                    }
                    Ctrl_Var.MovingForwardPuls = static_cast<long>(Ctrl_Var.dMovingForwardPuls);
                    //    guocheng = "条件1";
                }
                else if( Ctrl_Var.DealMethod_Ctrl == tagDealMethod::FINDTALEPIX){
                    int TalePixToBottom = Ctrl_Var.Tale_Pix_Ctrl + Ctrl_Var.Repeat_Height;//(CHECK_R2 - CHECK_R1) - (cv::boundingRect(Ctrl_Var.ModelContours.at(0)).height - Ctrl_Var.Repeat_Height);
                    if(TalePixToBottom < 0) return "异常，很可能检测搜索框的尺寸小或者模具读取参数异常";
                    double Puls =    -Y_RATIO * TalePixToBottom / Y_DIS_PULS_RATIO;
                    if((Puls < 0) && abs(Puls) > abs(Ctrl_Var.dMovingForwardPuls) )
                    {
                        Ctrl_Var.dMovingForwardPuls = Puls;
                    }
                    Ctrl_Var.MovingForwardPuls = static_cast<long>(Ctrl_Var.dMovingForwardPuls);
                    //   guocheng = "条件2";
                }

            }
            else
            {
                Ctrl_Var.dMovingForwardPuls = -Y_RATIO * (CHECK_R2- CHECK_R1 - 2.0*rtHegiht) / Y_DIS_PULS_RATIO;
                Ctrl_Var.MovingForwardPuls = static_cast<long>(Ctrl_Var.dMovingForwardPuls);
            }
        }
        else
        {

            Ctrl_Var.dMovingForwardPuls = -Y_RATIO * (0.75*(CHECK_R2- CHECK_R1)/* - 0.05*rtHegiht*/) / Y_DIS_PULS_RATIO;
            Ctrl_Var.MovingForwardPuls = static_cast<long>(Ctrl_Var.dMovingForwardPuls);
        }

        if (Ctrl_Var.dMovingForwardPuls >= 0)
        {
            return "模板太大，或者检测视野太小，请更换小的模具或者画大点搜索框";

        }
        return "OK";
    }

#if 0
    std::string Move_Punch(){ return "OK";}
#else

    //送料机移动,前移
    std::string Move_Punch(){
        try{
            std::string res;
            Ctrl_Var.ModelsPostion.sort([](std::vector<cv::Point2l> a, std::vector<cv::Point2l> b) {try { return a.at(0).y < b.at(0).y; } catch (std::out_of_range e) { throw e; }});
            res = PrepareMovePunchParameter(Ctrl_Var);

#ifdef NO_MOTION
            //测试下用的

            if(0 != strcmp("OK",res.c_str())) return res;
            for (std::list<std::vector<cv::Point2l>>::iterator it = Ctrl_Var.ModelsPostion.begin(); it != Ctrl_Var.ModelsPostion.end();)
            {
                //移动到了cc下面
                if ((Y_CAM_DISTANCE_PLS_EX - it->at(0).y) > -1)
                {
                    //进行排序，冲压最近的那个点
                    SortPunchVector(*it);
                    //只有冲压完毕才允许删除一个冲压点,保证存档正确
                    for (std::vector<cv::Point2l>::iterator point_it = it->begin(); point_it != it->end();point_it++)
                    {
                        show_Counter();
                    }
                    Ctrl_Var.ModelsPostion.erase(it++);
                }
                else
                {
                    ++it;
                }
            }
            printf("换行\n");
            //随机取消几行
            if(0 == rand()%3)
            {
                int cc =3;
                while(!Ctrl_Var.ModelsPostion.empty())
                {
                    Ctrl_Var.ModelsPostion.pop_front();
                    cc--;
                    if(cc < 0) break;
                }
            }

            int MinPuls = 0;
            for (std::list<std::vector<cv::Point2l>>::iterator it = Ctrl_Var.ModelsPostion.begin(); it != Ctrl_Var.ModelsPostion.end();it++)
            {
                if ((-Y_CAM_DISTANCE_PLS_EX - it->at(0).y) < 0)
                {
                    MinPuls = static_cast<int>(-Y_CAM_DISTANCE_PLS_EX - it->at(0).y);
                    Ctrl_Var.MovingForwardPuls = Ctrl_Var.MovingForwardPuls > MinPuls ? Ctrl_Var.MovingForwardPuls : MinPuls;
                    break;
                }
            }

            Ctrl_Var.CurrentYAxisPostion = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS, 1)-Ctrl_Var.MovingForwardPuls;
            for(int i = 0;i< 8;i++) motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, OFF);
            clock_t clk_move_record = clock();
            for(;false == isSnapOver || !isAxisStop(Y_AXIS_MOTOR);)  {
                if((clock()-clk_move_record)>10000)
                    return "Y轴移动过慢，或者报警，或者相机拍摄超时";
                if(STOP == (STOP&MachineState))
                {
                    return "按下停止按钮，停止冲压";
                }
            }
            char Msg[256] = {0};
            sprintf(Msg,"应该前进的脉冲为:%d",Ctrl_Var.MovingForwardPuls);
            YNeedMovePuls = abs(Ctrl_Var.MovingForwardPuls);
            UpdateMessage(Msg);

            return "OK";
#else
            size_t ModelsPostionSize = Ctrl_Var.ModelsPostion.size();
            static size_t Old_ModelsPostionSize = ModelsPostionSize;
            clock_t clk_move_record = clock();
            res = MoveBeforePunch(ModelsPostionSize, Old_ModelsPostionSize, Ctrl_Var,clk_move_record);
            if(0 != strcmp("OK",res.c_str())) return res;

            for (std::list<std::vector<cv::Point2l>>::iterator it = Ctrl_Var.ModelsPostion.begin(); it != Ctrl_Var.ModelsPostion.end();)
            {
                if ((-Y_CAM_DISTANCE_PLS_EX - it->at(0).y) > -1)
                {
                    //进行排序，冲压最近的那个点
                    SortPunchVector(*it);
                    std::list<cv::Point2l> Tmp;
                    //只有冲压完毕才允许删除一个冲压点,保证存档正确
                    for (std::vector<cv::Point2l>::iterator point_it = it->begin(); point_it != it->end();point_it++)
                    {
                        Tmp.push_back(*point_it);
                    }
                    for (std::list<cv::Point2l>::iterator point_it = Tmp.begin(); point_it != Tmp.end();)
                    {
                        long x_move_puls = X_CAM_DISTANCE_PLS - point_it->x;
                        //()
                        //逻辑，这个是在点读到的是不在远点
                        motion::GetIns()->CurrentCard()->absolute_move(X_AXIS_MOTOR, x_move_puls, 50, X_AXIS_SPEED);
                        clk_move_record = clock();
                        //需要改善,这个地方有暂停的bug
                        //条件是。。。(X轴和Y轴的欧式距离小于)||(X轴欧式距离小于或者Y轴停止)
                        //for (; false == WaitAxisDone(X_AXIS_MOTOR, 10000,POS_ACCURCY,x_move_puls) || false == isAxisStop(Y_AXIS_MOTOR);)
                        while(true)
                        {

#if 0
                            int YDis = YNeedMovePuls > 0?YNeedMovePuls:0;
                            if(YNeedMovePuls == -1) YDis = 99999999;
                            YNeedMovePuls = 1.2*YNeedMovePuls;
                            int XDis = abs(static_cast<int>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,X_AXIS_MOTOR) - x_move_puls));
                            double EuclideanDistance = std::sqrtf(static_cast<double>(YDis)*static_cast<double>(YDis) + static_cast<double>(XDis)*static_cast<double>(XDis));
                            if((EuclideanDistance <= abs(POS_ACCURCY)) ||
                                    (XDis <=  abs(POS_ACCURCY) && true == isAxisStop(Y_AXIS_MOTOR)))
#else
                            int XDis = abs(static_cast<int>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,X_AXIS_MOTOR) - x_move_puls));
                            if((XDis <=  abs(POS_ACCURCY) && true == isAxisStop(Y_AXIS_MOTOR)))
#endif

                            {
                                break;
                            }

                            if((clock() - clk_move_record) > 10000) return "X轴或者Y轴运动超时，电机报警或者速度过慢";
                            if(PAUSE == (PAUSE&MachineState))
                            {
                                return "OK";
                            }
                            Sleep(3);
                        }
                        //有毛用？
                        Ctrl_Var.CurrentXAxisPostion = x_move_puls;
                        //进行冲压		，检查是不是没有运动
                        if(!CheckPunchTimeOut())           { return "六秒内没有发现冲床运动，件请检查通气通电状态和感应器是否安装正确。";}
                        show_Counter();
                        Tmp.erase(point_it++);
                        //中断应该放在这个地方
                        if(PAUSE == (PAUSE&MachineState))
                        {
                            if(!Tmp.empty())
                                for (auto Pt:Tmp)
                                {
                                    it->clear();
                                    it->push_back(Pt);

                                }
                            else
                            {
                                Ctrl_Var.ModelsPostion.erase(it++);
                            }
                            return "OK";
                        }
                        else if(STOP == (STOP&MachineState))
                        {
                            return "按下停止按钮，停止冲压";
                        }
                    }//冲压点for循环
                    Ctrl_Var.ModelsPostion.erase(it++);
                }
                else
                {
                    ++it;
                }
            }

            int MinPuls = 0;
            for (std::list<std::vector<cv::Point2l>>::iterator it = Ctrl_Var.ModelsPostion.begin(); it != Ctrl_Var.ModelsPostion.end();it++)
            {
                if ((-Y_CAM_DISTANCE_PLS_EX - it->at(0).y) < 0)
                {
                    MinPuls = static_cast<int>(-Y_CAM_DISTANCE_PLS_EX - it->at(0).y);
                    Ctrl_Var.MovingForwardPuls = Ctrl_Var.MovingForwardPuls > MinPuls ? Ctrl_Var.MovingForwardPuls : MinPuls;
                    break;
                }
            }

            Ctrl_Var.CurrentYAxisPostion = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS, 1)-Ctrl_Var.MovingForwardPuls;
            for(int i = 0;i< 8;i++) motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, OFF);
            clk_move_record = clock();
            for(;false == isSnapOver || !isAxisStop(Y_AXIS_MOTOR);)  {
                if((clock()-clk_move_record)>10000)
                    return "Y轴移动过慢，或者报警，或者相机拍摄超时";
                if(STOP == (STOP&MachineState))
                {
                    return "按下停止按钮，停止冲压";
                }
            }

#ifdef HIGHPERFOR
            if(POS_ACCURCYY > 100 && POS_ACCURCYY>abs(Ctrl_Var.MovingForwardPuls))
            {
                motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, ON);
            }
#endif

            char Msg[256] = {0};
            sprintf(Msg,"应该前进的脉冲为:%d",Ctrl_Var.MovingForwardPuls);
            YNeedMovePuls = abs(Ctrl_Var.MovingForwardPuls);
            UpdateMessage(Msg);
            motion::GetIns()->CurrentCard()->relative_move(Y_AXIS_MOTOR, -Ctrl_Var.MovingForwardPuls, 20, Y_AXIS_SPEED);
#endif
        }catch(std::out_of_range ex)
        {
            return ex.what();
        }
        catch(cv::Exception ex)
        {
            return ex.what();
        }
        return "OK";
    }
#endif

    void IO_Scanner(){
#ifdef NO_MOTION
        return;
#endif
        int x_new_bit = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & AXIS_ALM;
        static int x_old_bit = x_new_bit;
        int y_new_bit = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO, 1) & AXIS_ALM;
        static int y_old_bit = y_new_bit;
        int press_start_pause_button = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & YANWEI_ORIGIN_BIT;
        static int old_press_start_pause_button = press_start_pause_button;

        if (x_new_bit && x_new_bit != x_old_bit) {
            MachineState |= (XAXISALM | NOT_ORIGIN | STOP);
            MachineOp(MACHINE_STOP);
            UpdateMessage("X轴伺服报警,请切完伺服电源后5s，再接通电源");
        }
        else if(!x_new_bit)
        {
            MachineState &= ~XAXISALM;
        }

        if (y_new_bit && y_new_bit != y_old_bit) {
            MachineState |= (STOP);
            MachineOp(MACHINE_STOP);
            UpdateMessage("Y轴伺服报警，请切完伺服电源后5s，再接通电源");

        }else if(!y_new_bit)
        {
            MachineState &= ~YAXISALM;
        }

        if (press_start_pause_button && press_start_pause_button != old_press_start_pause_button)
        {
            Mediator::GetIns()->MachineState &= ~ORIGING;
            if(PAUSE == (PAUSE&MachineState))
            {
                MachineOp(MACHINE_START);
            }
            else if(RUN == (RUN&MachineState)){
                MachineOp(MACHINE_PAUSE);
            }
        }
        x_old_bit = x_new_bit;
        y_old_bit = y_new_bit;
        old_press_start_pause_button = press_start_pause_button;
    }

    bool isXInDistance(short Axis, int DestPos, int Accurcy = -1)
    {
        if (Accurcy < 0) return true;
        return (Accurcy >= abs(DestPos - static_cast<long long>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Axis))));
    }

    bool isAxisStop(short Axis, long long Accurcy = 50)
    {
        return 0 == motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_STATE, Y_AXIS_MOTOR);
        long long Pos1 = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS, Axis);
        Sleep(10);
        long long Pos2 = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS, Axis);
        return (abs(Pos1 - Pos2) < Accurcy);
    }

    bool isAxisStopEx(short Axis, long long pos, long long accurcy = -1){
        if (accurcy < 0) return true;
        return (accurcy >= abs(pos - static_cast<long long>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Axis))));

    }
    //转化为脉冲
    bool WaitAxisDone(short Axis = 0, clock_t TimeOut = 2000, int AxisAccury = -2, int DesPos = 0, long long & Command = Mediator::GetIns()->MachineState)
    {
        clock_t clk = clock();
        if(AxisAccury < -1)
        {
            while(0 != motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_STATE, Axis))
            {
                if(STOP == (STOP&Command))
                {
                    return true;
                }

                if((clock()-clk) > TimeOut)
                {
                    return false;
                }
                Sleep(2);
            }
            return true;
        }
        else
        {
            if (AxisAccury < 0) return true;
            while(AxisAccury < abs(DesPos - static_cast<long>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Axis))))
            {
                if(STOP == (STOP&Command))
                {
                    return true;
                }
                if((clock()-clk) > TimeOut)
                {
                    return false;
                }
                Sleep(2);
            }
            return true;
        }
    }

    void Close()
    {
        MachineOp(MACHINE_STOP);
    }

    void reset_data(){
        Ctrl_Var.CurrentXAxisPostion = 0;
        Ctrl_Var.CurrentYAxisPostion = 0;
        Ctrl_Var.dMovingForwardPuls = 0;
        Ctrl_Var.MovingForwardPuls = 0;
        Ctrl_Var.Cal_ModelsPostion.clear();
        Ctrl_Var.ModelsPostion.clear();
        Ctrl_Var.margin_to_model = BOARD_DISTANCE;
        Ctrl_Var.model_distance = MODEL_GAP;
        Ctrl_Var.image_threshold = Get_Obj_Threshold;
        cv::Point2d mm(POS_ACCURCY, POS_ACCURCY); cv::Point pls;
        Excv::mm_to_pls(mm, pls);
        Ctrl_Var.Pos_Acc = pls.x;
        Ctrl_Var.Lines_Method = LINE_METHOD;
        if(!Ctrl_Var.Buffer.empty()) Ctrl_Var.Buffer.release();
        Ctrl_Var.DealMethod_Ctrl = tagDealMethod::NORMAL;
        Ctrl_Var.ROI.x = CHECK_C1;
        Ctrl_Var.ROI.y = CHECK_R1;
        Ctrl_Var.ROI.width = CHECK_C2 - CHECK_C1;
        Ctrl_Var.ROI.height = CHECK_R2 - CHECK_R1;
        Ctrl_Var.VertialSkipRatio = VERTICAL_SKIP_RATIO;
        Ctrl_Var.ReturnRatio = RETURNRATIO;
        Virtual_Image = cv::Mat(Ctrl_Var.ROI.height*2,Ctrl_Var.ROI.width,CV_8UC1,cv::Scalar(255,255,255));
    }

    long long MachineOp(int sel = 0, int AuxPara = -1, int SecondAux = -2)
    {
        static std::mutex mtx;
        std::lock_guard<std::mutex> lck(mtx);
        static int LastButtonState = STOP;
        switch (sel) {
        case MOVE_UP:
#ifndef NO_MOTION
            if(STOP != (STOP&MachineState) || YAXISALM == (YAXISALM & MachineState))
            {
                UpdateMessage("没有停下来或者Y轴伺服报警");return MachineState;
            }
#endif
            if(SecondAux <0)
                motion::GetIns()->CurrentCard()->v_move(Y_AXIS_MOTOR, Y_AXIS_SPEED);
            else
            {
                motion::GetIns()->CurrentCard()->absolute_move(Y_AXIS_MOTOR, AuxPara,Y_AXIS_SPEED);
            }
            break;

        case MOVE_LEFT:
#ifndef NO_MOTION
            if(STOP != (STOP&MachineState) || ORIGING == (ORIGING & MachineState) || XAXISALM == (XAXISALM & MachineState))
            {
                UpdateMessage("没有停下来或者X轴伺服报警");return MachineState;
            }
#endif
            if(SecondAux <0)
                motion::GetIns()->CurrentCard()->v_move(X_AXIS_MOTOR, -X_AXIS_SPEED);
            else
            {
                motion::GetIns()->CurrentCard()->absolute_move(X_AXIS_MOTOR, -AuxPara,X_AXIS_SPEED);
            }
            break;

        case MOVE_RIGHT:
#ifndef NO_MOTION
            if(STOP != (STOP&MachineState) || ORIGING == (ORIGING & MachineState) || XAXISALM == (XAXISALM & MachineState))
            {
                UpdateMessage("没有停下来或者X轴伺服报警");return MachineState;
            }
#endif
            if(SecondAux <0)
                motion::GetIns()->CurrentCard()->v_move(X_AXIS_MOTOR, X_AXIS_SPEED);
            else
            {
                motion::GetIns()->CurrentCard()->absolute_move(X_AXIS_MOTOR, AuxPara,X_AXIS_SPEED);
            }
            break;

        case MOVE_DOWN:
#ifndef NO_MOTION
            if(STOP != (STOP&MachineState) || YAXISALM == (YAXISALM & MachineState))
            {
                UpdateMessage("没有停下来或者Y轴伺服报警");return MachineState;
            }
#endif
            if(SecondAux <0)
                motion::GetIns()->CurrentCard()->v_move(Y_AXIS_MOTOR, -Y_AXIS_SPEED);
            else
            {
                motion::GetIns()->CurrentCard()->absolute_move(Y_AXIS_MOTOR, -AuxPara,Y_AXIS_SPEED);
            }
            break;
        case RELATIVE_MOVE_UP:
            if(STOP != (STOP&MachineState) || YAXISALM == (YAXISALM & MachineState))
            {
                UpdateMessage("没有停下来或者Y轴伺服报警");return MachineState;
            }
            motion::GetIns()->CurrentCard()->relative_move(Y_AXIS_MOTOR, AuxPara,Y_AXIS_SPEED);
            break;
        case RELATIVE_MOVE_DOWN:
            if(STOP != (STOP&MachineState) || YAXISALM == (YAXISALM & MachineState))
            {
                UpdateMessage("没有停下来或者Y轴伺服报警");return MachineState;
            }
            motion::GetIns()->CurrentCard()->relative_move(Y_AXIS_MOTOR, -AuxPara,Y_AXIS_SPEED);
            break;
        case RELATIVE_MOVE_LEFT:
            if(STOP != (STOP&MachineState) || XAXISALM == (XAXISALM & MachineState))
            {
                UpdateMessage("没有停下来或者X轴伺服报警");return MachineState;
            }
            motion::GetIns()->CurrentCard()->relative_move(X_AXIS_MOTOR, -AuxPara,X_AXIS_SPEED);
            break;
        case RELATIVE_MOVE_RIGHT:
            if(STOP != (STOP&MachineState) || XAXISALM == (XAXISALM & MachineState))
            {
                UpdateMessage("没有停下来或者X轴伺服报警");return MachineState;
            }
            motion::GetIns()->CurrentCard()->relative_move(X_AXIS_MOTOR, AuxPara,X_AXIS_SPEED);
            break;

        case MOVE_STOP:
            if(STOP != (STOP&MachineState))
            {
                UpdateMessage("送料机运动中，无效操作");return MachineState;
            }
            motion::GetIns()->CurrentCard()->e_stop(-1);
            break;

        case MACHINE_START:
            printf("Process 状态 %d \n",evt_Process.State());
            if(0 != strcmp("OK",PreparePara().c_str())){
                ERRORLOG(PreparePara());
                return MachineState;
            }else if(PUNCH_COUNTER > PUNCH_LIMIT && (0 < PUNCH_LIMIT))
            {
                ERRORLOG("达到最大冲压次数");
                return MachineState;
            }

            if(PUNCHMODE == DIGITALMODE)
            {
                try{
                    DxfPoints = mdxf.GetDxfPoints(Mediator::GetIns()->MainWindowDispHd);
                    if(DxfPoints.empty())
                    {
                        UpdateMessage("DXF文件没读取到点");
                        return MachineState;
                    }
                }
                catch(HalconCpp::HException e)
                {
                    UpdateMessage("DXF文件没读取到点");
                    return MachineState;
                }
                catch(MException &e)
                {
                    UpdateMessage(e.what());
                    return MachineState;
                }
            }

#ifndef NO_MOTION
            if(!CheckBeforStart()) return MachineState;
#endif
            MachineState &= ~(STOP|PAUSE);          MachineState |= RUN;
            if(PAUSE == LastButtonState)            {if(evt_Memento.State()) { UpdateMessage("存档中2...");return MachineState;}memento.Load(Ctrl_Var);}
            else if(STOP == LastButtonState)        reset_data();
            //获取当前轴的位置
            Ctrl_Var.CurrentXAxisPostion = static_cast<int>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS));
            Ctrl_Var.CurrentYAxisPostion = static_cast<int>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Y_AXIS_MOTOR));
            evt_Process.SetEvent();
            LastButtonState = RUN;
            show_StartButton();
            break;

        case MACHINE_PAUSE:
            //仅仅允许停止X轴
            YNeedMovePuls  = -1;
            evt_Process.ResetEvent();
            motion::GetIns()->CurrentCard()->e_stop_ex();
            MachineState &= ~(STOP|RUN);            MachineState |= PAUSE;
            if(RUN == LastButtonState)              {evt_Memento.SetEvent();}
            LastButtonState = PAUSE;

            show_StartButton();
            break;

        case MACHINE_STOP:
            printf("按下了停止键\n");
            YNeedMovePuls = -1;
            evt_Process.ResetEvent();
            //	 || 0 != motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_STATE, Y_AXIS_MOTOR);)
            motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, OFF);
            motion::GetIns()->CurrentCard()->e_stop_ex(-1);
            MachineState &= ~(RUN|PAUSE);           MachineState |= STOP;
            LastButtonState = STOP;
            show_StartButton();
            break;

        case MACHINE_ORIGIN:
            XAxisOrigin();
            break;
        case MACHINE_PUNCH:
            if(STOP == (STOP&MachineState) || PAUSE == (PAUSE&MachineState))
            {
                motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, ON); std::cout<<"punching" <<std::endl;
            }
            else
            {
                UpdateMessage("没有停下来"); return MachineState;
            }
            break;

        case MACHINE_SEPRA:
            if(STOP == (STOP&MachineState) || PAUSE == (PAUSE&MachineState))
            {
                motion::GetIns()->CurrentCard()->WriteOutput(OUT_PUNCH_CYL, OFF);std::cout<<"stop punching" <<std::endl;
            }
            else
            {
                ERRORLOG("没有停下来"); return MachineState;
            }
            break;

        default:
            break;
        }
        return MachineState;
    }

    void XAxisOrigin()
    {
        if(STOP != (STOP&MachineState) ||
                ORIGING == (ORIGING & MachineState) ||
                XAXISALM == (XAXISALM & MachineState))
        {
            UpdateMessage("没有停下来或者X轴伺服报警");
            return;
        }
        MachineState |= ORIGING;
        (HANDLE)_beginthreadex(NULL, 0,XAixsOriginThread, this, 0, NULL);
    }

    static unsigned int __stdcall XAixsOriginThread(void* pLVOID){
        Mediator::GetIns()->MachineState |= (ORIGING|NOT_ORIGIN);
        clock_t clk = clock();
        //读到上限就停止

        if(左方向为原点)
        {

            if (YANWEI_DOWN_LIMIT == (motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & YANWEI_DOWN_LIMIT))
            {
                motion::GetIns()->CurrentCard()->v_move(X_AXIS_MOTOR, -X_AXIS_ORIGIN_SPEED / 2);
            }

            for (; (YANWEI_DOWN_LIMIT == (motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & YANWEI_DOWN_LIMIT));)
            {
                if (ORIGING != (Mediator::GetIns()->MachineState & ORIGING)) {
                    goto endOrigin;
                }
                if ((clock() - clk) > 15000)
                {
                    goto endOrigin;
                }
                Sleep(1);
            }
            std::cout << "正在 停止" << std::endl;
            motion::GetIns()->CurrentCard()->e_stop(-1);
            Sleep(1500);
            clk = clock();

            if (YANWEI_DOWN_LIMIT != (motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & YANWEI_DOWN_LIMIT)) {
                motion::GetIns()->CurrentCard()->v_move(X_AXIS_MOTOR, X_AXIS_ORIGIN_SPEED / 20);
            }
            for (; (YANWEI_DOWN_LIMIT != (motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & YANWEI_DOWN_LIMIT));)
            {
                if (ORIGING != (Mediator::GetIns()->MachineState & ORIGING))  {
                    goto endOrigin;
                }
                if ((clock() - clk) > 5000){
                    goto endOrigin;
                }
                //Sleep(1);
            }

        }
        else
        {
            if (YANWEI_UP_LIMIT == (motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & YANWEI_UP_LIMIT))
            {
                motion::GetIns()->CurrentCard()->v_move(X_AXIS_MOTOR, X_AXIS_ORIGIN_SPEED / 2);
            }

            for (; (YANWEI_UP_LIMIT == (motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & YANWEI_UP_LIMIT));)
            {
                if (ORIGING != (Mediator::GetIns()->MachineState & ORIGING)) {
                    goto endOrigin;
                }
                if ((clock() - clk) > 15000)
                {
                    goto endOrigin;
                }
                Sleep(1);
            }
            std::cout << "正在 停止" << std::endl;
            motion::GetIns()->CurrentCard()->e_stop(-1);
            Sleep(1500);
            clk = clock();

            if (YANWEI_UP_LIMIT != (motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & YANWEI_UP_LIMIT)) {
                motion::GetIns()->CurrentCard()->v_move(X_AXIS_MOTOR, -X_AXIS_ORIGIN_SPEED / 20);
            }
            for (; (YANWEI_UP_LIMIT != (motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & YANWEI_UP_LIMIT));)
            {
                if (ORIGING != (Mediator::GetIns()->MachineState & ORIGING))  {
                    goto endOrigin;
                }
                if ((clock() - clk) > 5000){
                    goto endOrigin;
                }
                //Sleep(1);
            }

        }
        Mediator::GetIns()->MachineState &= ~NOT_ORIGIN;
        Mediator::GetIns()->MachineOp(MACHINE_STOP);
        Sleep(1000);
        motion::GetIns()->CurrentCard()->SetAxisZero();
endOrigin:
        Mediator::GetIns()->MachineOp(MACHINE_STOP);
        if (NOT_ORIGIN != (NOT_ORIGIN & Mediator::GetIns()->MachineState)) {
            Mediator::GetIns()->UpdateMessage("复位成功!");
        }
        else
        {
            Mediator::GetIns()->UpdateMessage("复位超时，可能原因电机过慢，电机报警!");
        }
        Mediator::GetIns()->MachineState &= ~ORIGING;
        return 0;
    }


    std::string PreparePara()
    {
        if(evt_Memento.State())             return "存档中...";
        if(Ctrl_Var.ModelContours.empty())  return "没有载入模具图，无法计算";
        Ctrl_Var.ROI = cv::Rect(CHECK_C1,CHECK_R1,CHECK_C2 - CHECK_C1, CHECK_R2 - CHECK_R1);
        double Width_Check = CHECK_C2-CHECK_C1;
        double Hegiht_Check = CHECK_C2 - CHECK_C1;
        cv::Rect Rt = cv::boundingRect(Ctrl_Var.ModelContours.at(0));
        //以后需要增强，只判断宽度
        if(Width_Check <= Rt.width || Hegiht_Check <= Rt.height)        return "搜索框太小";
        return std::string("OK");
    }
    //计算时候需要检查下控制状态

    bool CheckBeoreCal()
    {
        bool isOK = true;
        if(Ctrl_Var.ModelContours.empty())
        {
            isOK = false; ERRORLOG("没有载入模具图，无法计算");
        }
        //检查搜索框
        double Width_Check = CHECK_C2-CHECK_C1;
        double Hegiht_Check = CHECK_C2 - CHECK_C1;
        cv::Rect Rt = cv::boundingRect(Ctrl_Var.ModelContours.at(0));
        if(Width_Check <= Rt.width || Hegiht_Check <= Rt.height)
        {
            isOK = false;
            ERRORLOG("搜索框太小");
        }
        return isOK;
    }

    //启动前需要检查机器的状态
    bool CheckBeforStart()
    {
        //参数是不进行报警的意思
        bool isOK = true;
        isOK = CheckBeoreCal();
        if(evt_GetPoint.State()){
            isOK = false;
            ERRORLOG("没有计算完毕");
        }
        // if(evt_Punch.State()){
        //     isOK = false;
        //     UpdateMessage("似乎冲床没有停下来");
        //}

        if(MSerialsCamera::camera_number() < 1)
        {
            isOK = false;
            MachineState |= NO_CAMERA;
            ERRORLOG("没有发现相机");
        }
        else
            MachineState &= ~NO_CAMERA;


        if(NO_MOTION_CARD == (NO_MOTION_CARD&MachineState))
        {
            isOK = false;
            ERRORLOG("没有发现控制卡");
        }

        int x_new_bit = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO) & AXIS_ALM;
        int y_new_bit = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_IO, 1) & AXIS_ALM;

        if(x_new_bit){
            isOK = false;
            MachineState |= XAXISALM;
            ERRORLOG("X轴报警");
        }
        else
        {
            MachineState &= ~XAXISALM;
        }
        //伺服解除报警
        if (y_new_bit)
        {
            isOK = false;
            MachineState |= YAXISALM;
            ERRORLOG("Y轴报警");
        }
        else
        {
            MachineState &= ~YAXISALM;
        }

        if (OUT_PUNCH_CYL == (motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_OUT_IO_SEL) & OUT_PUNCH_CYL))
        {
            isOK = false;
            MachineState |= ERROR_PUNCHING;
            ERRORLOG("未能检测到感应器，冲床没有抬起？");
        }
        else
        {
            MachineState &= ~ERROR_PUNCHING;
        }

        if(NOT_ORIGIN & MachineState)
        {
            isOK = false;
            ERRORLOG("没有复位过");
        }

        if(ORIGING == (ORIGING & MachineState))
        {
            isOK = false;
            ERRORLOG("复位中...");
        }
        return isOK;
    }

    void SetDxfPara(double hl, double hu, double wl, double wu)
    {
        mdxf.SetPara(hl,hu,wl,wu);
    }


    bool Load_Dxf(std::string file_name, HTuple window = 0, bool save = false)
    {
        int64 winh = window;
        HObject Dxf;
        HalconCpp::GenEmptyObj(&Dxf);
        try{
            std::cout<<"read path is "<<file_name<<std::endl;
            Dxf = mdxf.ReadDxf(file_name.c_str(), winh);
            DxfContours = Dxf;
            if(save)
            {
                DXFPATH = QString::fromLocal8Bit(file_name.c_str());
                PRJSAVE;
            }
            return true;
        }catch(MException &e)
        {
            printf(e.what());
            MException error("File Dxf Error\n");
            throw error;
        }
    }



    bool Load_Model(std::string file_name, HTuple Window = 0, bool Save = true)
    {
        std::cout << "文件名:" << file_name << std::endl;
        cv::Mat image = cv::imread(file_name);
        if (image.empty())
        {
            QMessageBox::information(NULL,"Error",QString::fromLocal8Bit("没有读取到图片？"));
            return false;
        }
        try {
            cv::Mat single_Model, b_Model;
            CvCvtColor(image, single_Model, CV_BGR2GRAY);
            cv::threshold(single_Model, b_Model, 128, 255, CV_THRESH_BINARY);
            std::vector<std::vector<cv::Point>> contours,tmp_cotours;
            cv::Mat add_Border;
            int AddHeightBorder = (IMAGE_HEIGHT - b_Model.rows)/2 < 2? 2 :(IMAGE_HEIGHT - b_Model.rows)/2;
            int AddWidthBorder = (IMAGE_WIDTH - b_Model.cols)/2 < 2 ? 2: (IMAGE_WIDTH - b_Model.cols)/2;
            cv::copyMakeBorder(b_Model, add_Border,  AddHeightBorder,AddHeightBorder, AddWidthBorder, AddWidthBorder, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
            Excv::MatToHObj(add_Border, HalconImage);
            cv::findContours(add_Border, tmp_cotours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
            if (tmp_cotours.empty())
            {
                QMessageBox::information(NULL,"Error",QString::fromLocal8Bit("没有找到模具"));
                return false;
            }
            int max_id = 0;
            size_t max_size = tmp_cotours.at(0).size();
            for(int i = 0;i < tmp_cotours.size();i++)
            {
                if(max_size < tmp_cotours[i].size())
                {
                    max_size = tmp_cotours[i].size();
                    max_id = i;
                }
            }
            contours.push_back(tmp_cotours[max_id]);
            cv::Rect BoundingBox = cv::boundingRect(contours.at(0));
            cv::Mat color_image;
            Excv::Contours_Move(contours,Ctrl_Var.ModelContours, cv::Point(-BoundingBox.x,-BoundingBox.y));
            CvCvtColor(add_Border, color_image, CV_GRAY2BGR);
            cv::drawContours(color_image, contours, -1/*获得的轮廓序号*/, cv::Scalar(255, 128, 64), 1,cv::LINE_AA);
#ifdef NO_MOTION
            cv::drawContours(color_image, Ctrl_Var.ModelContours, -1/*获得的轮廓序号*/, cv::Scalar(255, 68, 255), 1);
#endif
            image = color_image.clone();
            HalconCpp::HObject hobj;
            Excv::MatToHObj(image,hobj);
            if( 0!=Window) Excv::h_disp_obj(hobj,Window);
            Preference::GetIns()->prj->Model_Name = QString::fromLocal8Bit(file_name.c_str());
            if(Save) Preference::GetIns()->prj->WriteSettings();
        }
        catch (cv::Exception e)
        {
            QMessageBox::information(NULL,"Error",QString::fromLocal8Bit("设置失败，没有发现轮廓！"));
            return false;
        }
        return true;
    }




    //这个函数应该取
    //drawout 代表是要填充的，用于计算而不是显示，反之是显示用的，不是计算用的
    bool  DrawPts(cv::Mat& InputArryay, Control_Var &Ctrl_Var, bool isDrawOut = false)
    {
        MASSERT(!InputArryay.empty() && !Ctrl_Var.ModelContours.empty());
        std::vector<std::vector<cv::Point>> contours;
        static bool changeColor = false;
        cv::Scalar Color = cv::Scalar(255,128,0);
        switch (LINE_METHOD)
        {
        case LINES_HORIZONTAL_FILL:
            contours = Ctrl_Var.ModelContours;
            for (auto &vector_points : Ctrl_Var.ModelsPostion)
            {
                for (auto &p : vector_points)
                {
                    cv::Point pt;
                    //异常
                    Excv::pls_to_pix(p, pt);
                    if (pt.y > Ctrl_Var.Tale_Pix) { Ctrl_Var.Tale_Pix = pt.y; }
                    if(isDrawOut)
                    {
                        pt.y = pt.y - Ctrl_Var.ROI.y + Ctrl_Var.ROI.height;
                        pt.x = pt.x - Ctrl_Var.ROI.x;
                    }
                    if (isDrawOut) cv::drawContours(InputArryay, contours, 0/*获得的轮廓序号*/, cv::Scalar(255, 128, 0), Ctrl_Var.model_distance, cv::LINE_4, cv::noArray(), INT_MAX, pt);
                    cv::drawContours(InputArryay, contours, 0/*获得的轮廓序号*/, Color, -1, cv::LINE_4, cv::noArray(), INT_MAX, pt);
                }
            }
            break;
        case LINES_HORIZONTAL_USER:
            contours = Ctrl_Var.ModelContours;
            for (auto &vector_points : Ctrl_Var.ModelsPostion)
            {
                for (auto &p : vector_points)
                {
                    cv::Point pt;
                    //异常
                    Excv::pls_to_pix(p, pt);
                    if (pt.y > Ctrl_Var.Tale_Pix) { Ctrl_Var.Tale_Pix = pt.y; }
                    if(isDrawOut)
                    {
                        pt.y = pt.y - Ctrl_Var.ROI.y + Ctrl_Var.ROI.height;
                        pt.x = pt.x - Ctrl_Var.ROI.x;
                    }
                    if (isDrawOut) cv::drawContours(InputArryay, contours, 0/*获得的轮廓序号*/, cv::Scalar(255, 128, 0), Ctrl_Var.model_distance, cv::LINE_4, cv::noArray(), INT_MAX, pt);
                    cv::drawContours(InputArryay, contours, 0/*获得的轮廓序号*/, Color, -1, cv::LINE_4, cv::noArray(), INT_MAX, pt);
                }
            }
            break;


#if 0
        case DOUBLE_HORIZONTAL:
            contours = CvGetDoubleContoursHorizental_Ex(Ctrl_Var.ModelContours);
            for (auto &vector_points : Ctrl_Var.ModelsPostion)
            {
                for (auto &p : vector_points)
                {
                    cv::Point pt;
                    Excv::pls_to_pix(p, pt);
                    if (pt.y > Ctrl_Var.Tale_Pix) { Ctrl_Var.Tale_Pix = pt.y;
                    }
                    if (isDrawOut) cv::drawContours(InputArryay, contours, 0/*获得的轮廓序号*/, cv::Scalar(255, 128, 0), Ctrl_Var.model_distance, cv::LINE_4, cv::noArray(), INT_MAX, pt);

                    cv::drawContours(InputArryay, contours, 0/*获得的轮廓序号*/, Color, -1, cv::LINE_4, cv::noArray(), INT_MAX, pt);
                }
            }
            break;
#endif
        default:
            contours = Ctrl_Var.ModelContours;
            for (auto &vector_points : Ctrl_Var.ModelsPostion)
            {
                for (auto &p : vector_points)
                {
                    cv::Point pt;
                    //异常
                    Excv::pls_to_pix(p, pt);
                    if (pt.y > Ctrl_Var.Tale_Pix) { Ctrl_Var.Tale_Pix = pt.y; }
                    if (isDrawOut) cv::drawContours(InputArryay, contours, 0/*获得的轮廓序号*/, cv::Scalar(255, 128, 0), Ctrl_Var.model_distance, cv::LINE_4, cv::noArray(), INT_MAX, pt);
                    cv::drawContours(InputArryay, contours, 0/*获得的轮廓序号*/, Color, -1, cv::LINE_4, cv::noArray(), INT_MAX, pt);
                }
            }
            break;
        }
        //       / using namespace  cv;        __FILE__
        return true;
    }

};

#endif // MEDIATOR_H




















