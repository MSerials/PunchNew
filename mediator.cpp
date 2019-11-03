#include "mediator.h"

//unsigned int Mediator::InitCameraAndMotionCard(void* pLVOID);
//图像转换函数系列
//从控制系统的脉冲变成像素
void Mediator::PulsPos2PixPos(const std::list<std::vector<cv::Point2l>> & PulsPos, std::list<std::vector<cv::Point>> & PixPos, Control_Var Ctrl_Var){
    try{
        if(!PixPos.empty()) PixPos.clear();
        for(auto &vecPoint : PulsPos){
            std::vector<cv::Point> VPoints;
            for(auto &point :vecPoint){
                    cv::Point pt;
                    Excv::pls_to_pix(cv::Point(point.x,point.y), pt);
                    if(isFillMode(Ctrl_Var)){
                        pt.y = pt.y - Ctrl_Var.ROI.y + Ctrl_Var.ROI.height;
                        pt.x = pt.x - Ctrl_Var.ROI.x;
                    }
                    VPoints.push_back(pt);
            }
            PixPos.push_back((VPoints));
        }
    }catch(std::exception ex){
        throw std::move(ex);
    }
}

void Mediator::PixPos2PulsPos(const std::list<std::vector<cv::Point2l>> & PixPos, std::list<std::vector<cv::Point>> & PulsPos, Control_Var Ctrl_Var){
    try{
        if(!PulsPos.empty()) PulsPos.clear();
        for(auto &vecPoint : PixPos){
            std::vector<cv::Point> VPoints;
            for(auto &point :vecPoint){
                    cv::Point pt;
                    Excv::pix_to_pls(cv::Point(point.x,point.y), pt);
                    /*
                    if(LINES_HORIZONTAL_FILL == Ctrl_Var.Lines_Method ||
                            LINES_HORIZONTAL_USER == Ctrl_Var.Lines_Method ){
                    pt.y = pt.y - Ctrl_Var.ROI.y + Ctrl_Var.ROI.height;
                    pt.x = pt.x - Ctrl_Var.ROI.x;
                    }

                    pt.y = pt.y - Ctrl_Var.ROI.y + Ctrl_Var.ROI.height;
                    pt.x = pt.x - Ctrl_Var.ROI.x;
                    */
                    VPoints.push_back(pt);
            }
            PulsPos.push_back((VPoints));
        }
    }catch(std::exception &ex){
        throw ex;
    }
}


void Mediator::PrepareCalImageAndShowImage(cv::Mat & InputArray,cv::Mat & Cal_Image, cv::Mat & Show_Image, Control_Var & Ctrl_Var){
        MASSERT(!InputArray.empty() && InputArray.type() == CV_8UC1);
        PulsPos2PixPos(Ctrl_Var.ModelsPostion,Ctrl_Var.ModelsPositionPix,Ctrl_Var);
        //这里将Models的倒推一下，从脉冲数变成像素的
        cv::Mat InputArray2;
        int sized = static_cast<int>(Ctrl_Var.margin_to_model);
        cv::threshold(InputArray,InputArray2,Ctrl_Var.image_threshold,255,CV_THRESH_BINARY);
        CvCvtColor(InputArray,Show_Image,CV_GRAY2BGR);

/*
        Ctrl_Var.Tale_Pix   =   0;

        //注意备注,在确认计算前找到对应的点

        Ctrl_Var.Tale_ModelsPostion.clear();
        for (auto &vector_points : Ctrl_Var.ModelsPostion)
        {
            std::vector<cv::Point> tmp;
            for (auto &p : vector_points)
            {
                cv::Point pt;
                Excv::pls_to_pix(p, pt);
                tmp.push_back(pt);
            }
            Ctrl_Var.Tale_ModelsPostion.push_back(tmp);
        }
*/
//必须再绘制上次点之前进行腐蚀
        if (sized > 0)
        {
            cv::Size si(sized, sized);
            cv::Mat er, get_Struct = cv::getStructuringElement(cv::MORPH_RECT, si);
            cv::dilate(InputArray2, er, get_Struct);
            Cal_Image = er.clone();
        }
        else
        {
            Cal_Image = InputArray2.clone();
        }
        Ctrl_Var.VertialSkipRatio = VERTICAL_SKIP_RATIO;
        Ctrl_Var.ReturnRatio = RETURNRATIO;
}


void Mediator::IOC(){
    printf("ddd");
}

//双模具下填充增加点
void Mediator::AddPointUnderDoubleModleAndFill(Control_Var & Ctrl_Var){
    if(1==DOUBLEMODEL && isFillMode(Ctrl_Var))
    {
        //append        //找到最后一排点，并增加要冲的一个
        long long ModelHeight = cv::boundingRect(Ctrl_Var.ModelContours.at(0)).height + 2*Ctrl_Var.model_distance + 2*Ctrl_Var.margin_to_model;
        cv::Point pt;
        Excv::pix_to_pls(cv::Point(ModelHeight,ModelHeight),pt);

        if(AddOnePunch && !Ctrl_Var.ModelsPostion.empty()){
            AddOnePunch = false;
            std::vector<cv::Point2l> TalePoints = Ctrl_Var.ModelsPostion.back();
            for(auto &Pt : TalePoints){
                Pt.y += pt.y;
            }
            if(0!=pt.y){
            Ctrl_Var.ModelsPostion.push_back(TalePoints);
            char Msg[256] = {0};
            sprintf_s(Msg,"追加成功，新增%lld\n",ModelHeight);
            UpdateMessage(Msg);
            }
        }
    }
}

void Mediator::mergeImage(const cv::Mat & PreviousImage, const cv::Mat & CurrentImage, cv::Mat &Dst, cv::Mat & Show_Image, Control_Var &Ctrl_Var){
    int VirtualImageShouldHaveHeight = Ctrl_Var.ROI.height*2;
    int VirtualImageShouldHaveWidth = Ctrl_Var.ROI.width;
    cv::Mat WillAdded,WillAddShow;
    cv::Rect WillAddedROI;
    //reset_data时候会进行调整virtualimage的
    try{
    MASSERT(Virtual_Image.cols == VirtualImageShouldHaveWidth);
    MASSERT(Virtual_Image.rows == VirtualImageShouldHaveHeight);
   // Ctrl_Var.Lines_Method = LINES_HORIZONTAL_FILL;
    WillAddedROI.x = 0;
    WillAddedROI.y =  Ctrl_Var.ROI.height;
    WillAddedROI.width = Ctrl_Var.ROI.width;
    WillAddedROI.height = Ctrl_Var.ROI.height;
    CurrentImage(Ctrl_Var.ROI).copyTo(WillAdded);
    //检测willAdded来觉得是否进行添加尾部
    if(1 == DOUBLEMODEL){
        bool HasImage = false;
        static bool OldHasImage =  HasImage;
        cv::Mat ImageFound;
        cv::threshold(WillAdded,ImageFound,Ctrl_Var.image_threshold,255,CV_THRESH_BINARY_INV);
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(ImageFound, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
        size_t MaxSize = 60;
        size_t MaxFound = 0;
        for(auto &Cont:contours){
            if(MaxFound < Cont.size()){
                MaxFound = Cont.size();
            }
        }
        if(MaxFound >= MaxSize){
            HasImage = true;
        }

        char Msg[256] = {0};
        sprintf_s(Msg,"最大面积为%zd",MaxFound);
        if(!HasImage &&(HasImage != OldHasImage)){
               AddOnePunch = true;
               ERRORLOG(std::string(Msg) + " 双冲头模式下追加尾部点");

        }else{
            ERRORLOG(std::string(Msg));
        }
        OldHasImage =  HasImage;
    }

    //这个是合成出来的图片
    MSerialsCamera::addImageToBigImage(Virtual_Image,WillAdded,WillAddedROI);
    Dst = Virtual_Image.clone();

    //也许有bug,bug在于没有进行X轴偏移，要减去x轴
    //新模式下不需要
    //DrawPts(Virtual_Image_Cal, Ctrl_Var, true);

    //对显示的进行填充 ROI超出和ROI没超出的
    WillAddedROI.height = 2*Ctrl_Var.ROI.height;
    if((Ctrl_Var.ROI.height+Ctrl_Var.ROI.y) < WillAddedROI.height){
         //截取virtual image的ROI高度应
        WillAddedROI.height = Ctrl_Var.ROI.height+Ctrl_Var.ROI.y;
         //节去vi的ROIy轴起始
        WillAddedROI.y = Ctrl_Var.ROI.height- Ctrl_Var.ROI.y;
        Virtual_Image(WillAddedROI).copyTo(WillAddShow);
        WillAddedROI.y = 0;
        WillAddedROI.x = Ctrl_Var.ROI.x;
        CvCvtColor(CurrentImage,Show_Image,CV_BGR2GRAY);
        MSerialsCamera::addImageToBigImage(Show_Image,WillAddShow,WillAddedROI);
        CvCvtColor(Show_Image,Show_Image,CV_GRAY2BGR);
    }
    //超出图像的填充
    else
    {
        WillAddedROI.y = Ctrl_Var.ROI.y - Ctrl_Var.ROI.height;
        WillAddedROI.x = Ctrl_Var.ROI.x;
        CvCvtColor(CurrentImage,Show_Image,CV_BGR2GRAY);
        MSerialsCamera::addImageToBigImage(Show_Image,Virtual_Image,WillAddedROI);
        CvCvtColor(Show_Image,Show_Image,CV_GRAY2BGR);
    }
    }catch(cv::Exception ex){
        throw std::exception(ex.what());
    }catch(std::exception &ex){
        throw ex;
    }
}

//2019.10.19
void Mediator::AddCalPosToModelsPos(std::list<std::vector<cv::Point>> & CalPos, std::list<std::vector<cv::Point2l>> & ModelsPos){
    for(auto &vpts: CalPos){
        std::vector<cv::Point2l> vts;
        for (auto &pt : vpts)
        {
            int x_ = pt.x* X_RATIO / X_DIS_PULS_RATIO;
            int y_ = pt.y* Y_RATIO / Y_DIS_PULS_RATIO;
            vts.push_back(cv::Point(x_, y_));
        }
        //push_back是带有拷贝性质的，这里必须拷贝，否则会造成bug
        ModelsPos.push_back(vts);
    }
}

//同步相机和运动
void Mediator::Process()
{
    try{
        Ctrl_Var.DealMethod = tagDealMethod::NORMAL;
        isSnapOver = false; evt_Punch.SetEvent(); evt_GetPoint.SetEvent();
        if(!Excv::WaitValue(evt_Punch.State(),AN_HOUR) || !Excv::WaitValue(evt_GetPoint.State(),AN_HOUR)){throw std::exception("超时了一个小时");}
        AddPointUnderDoubleModleAndFill(Ctrl_Var);                                                                                                 ///双模具冲头要多加一个点
        AddCalPosToModelsPos(Ctrl_Var.Cal_ModelsPostion,Ctrl_Var.ModelsPostion);                                                ///追加新图像的点
        DrawPts(Show_Image, Ctrl_Var, false);                                                                                                               ///绘制显示的点
        for (auto &vpts2 : Ctrl_Var.ModelsPostion) {for (auto &pt : vpts2){pt.y += Ctrl_Var.MovingForwardPuls;}}        ///对伺服控制进行便宜
        Ctrl_Var.Lines_Method = LINE_METHOD;
        if(isFillMode(Ctrl_Var)){
            int pix = 0;
            Excv::AxisYpls2pix(Ctrl_Var.MovingForwardPuls,pix);
            if(pix > 0) pix = -pix;
            cv::Mat t_mat = cv::Mat::zeros(2, 3, CV_32FC1);
            t_mat.at<float>(0, 0) = 1;
            t_mat.at<float>(0, 2) = 0; //水平平移量
            t_mat.at<float>(1, 1) = 1;
            t_mat.at<float>(1, 2) = pix; //竖直平移量
            cv::Mat src = Virtual_Image.clone();
            cv::warpAffine(src, Virtual_Image, t_mat, Virtual_Image.size());
        }
        double rtHegiht = static_cast<double>(cv::boundingRect(Ctrl_Var.ModelContours.at(0)).height);
        double rtROI = CHECK_R2 - CHECK_R1;
        //最后一个点加上repeat高度就是末端，如果末端大于。。。针对小于2倍模具的
        if (rtROI < ROI_MODEL_RATIO * rtHegiht)
        {
            Ctrl_Var.Header_Pix_Ctrl = Ctrl_Var.Header_Pix;
            if(!Ctrl_Var.Cal_ModelsPostion.empty()){
                Ctrl_Var.DealMethod = tagDealMethod::FINDTALEPIX;
                Ctrl_Var.Tale_Pix_Ctrl = Ctrl_Var.Cal_ModelsPostion.back().at(0).y;
            }
        }
        Ctrl_Var.DealMethod_Ctrl = Ctrl_Var.DealMethod;
        HalconCpp::HObject Hobj;
        Excv::MatToHObj(Show_Image,Hobj);
        Excv::h_disp_obj(Hobj,MainWindowDispHd);

        return;
    }catch(cv::Exception ex)
    {
        ERRORLOG("Error In Process" + std::string(ex.what()));
    }
    catch(MException e)
    {
        ERRORLOG("Error In Process" + std::string(e.what()));
    }
    catch(std::out_of_range ex){
        ERRORLOG("Error In Process" + std::string(ex.what()));
    }catch(std::exception ex){
        ERRORLOG(ex.what());
    }
    MachineOp(MACHINE_STOP);
}

//获取图像计算出来的点位
bool Mediator::Get_Points(std::list<std::vector<cv::Point>> &Points)
{
    try{
        if(false == WaitAxisDone(Y_AXIS_MOTOR,10000,-2,0,MachineState)) throw std::exception("Y轴运动超时,是否Y轴电机报警？");
#ifndef NO_MOTION
        MSerialsCamera::SnapTwice(Image,SNAP_DELAY);
#else
    Image = _global::GetIns()->Image.clone();
#endif
    isSnapOver = true;
    Ctrl_Var.Cal_ModelsPostion = Get_Points_Image(Image, Ctrl_Var.ModelContours, Ctrl_Var, LINE_METHOD);
    return true;
    }
    catch(std::out_of_range ex){
        ERRORLOG("错误2:" + std::string(ex.what()));
    }
    catch(cv::Exception ex){
        ERRORLOG("错误3:" + std::string(ex.what()));
    }catch(std::exception ex){
        ERRORLOG("错误1:" + std::string(ex.what()));
    }
     MachineOp(MACHINE_STOP);
     return false;
}


//控制过程选择，是视觉还是数控
unsigned int __stdcall Mediator::Process_(void* pLVOID){pLVOID  = nullptr;
   static clock_t maxTime = 0, coutt = 0;
    for(;;){
        ::WaitForSingleObject(Mediator::GetIns()->evt_Process.get(),INFINITE);
#ifdef NO_MOTION
        clock_t clk = clock();
        Mediator::GetIns()->Process();
        clock_t t = clock()-clk;
        coutt++;
        if((coutt)>80)
        {
            coutt = 0;
            maxTime = 0;
        }
        if(t > maxTime)
        {
            maxTime = t;
        }
        printf("循环时间%ld 最大循环时间 %ld %d\n",clock()-clk,maxTime,Mediator::GetIns()->evt_Process.State());
#else
        if(PUNCHMODE == CAMERAMODE)
        {
            Mediator::GetIns()->Process();
        }
        else
        {
            Mediator::GetIns()->DigitalProcess();
        }
#endif
    }
}
















//不常用代码
void Mediator::DigitalProcess()
{
    //副本
    //最初对位所使用的点
    cv::Point FirstPoint(0,0),ControlPoint(0,0);
    std::list<std::vector<cv::Point2d>> Points = DxfPoints;
    int Counter = 0;
    for(auto &pts:Points){
        for(auto &pt:pts){
            if( 0==pt.x) printf("left point\n");
            Counter++;
        }
    }
    if(Counter>0){
     cv::Point2d mm = Points.front().at(0);
     Excv::mm_to_pls(mm,FirstPoint);
    }

    long long lastCommand = MachineState;
    //每行1mm精度
    while(!Points.empty()){
        std::vector<cv::Point2d> pts = Points.front();
        for(auto &pt : pts){
            cv::Point WillPunched,WillMoved;
            Excv::mm_to_pls(pt,WillPunched);
            WillMoved.x = Ctrl_Var.CurrentXAxisPostion - (WillPunched.x - FirstPoint.x);
            WillMoved.y = Ctrl_Var.CurrentYAxisPostion + (WillPunched.y - FirstPoint.y);
            motion::GetIns()->CurrentCard()->absolute_move(X_AXIS_MOTOR, WillMoved.x, 50, X_AXIS_SPEED,2);
            motion::GetIns()->CurrentCard()->absolute_move(Y_AXIS_MOTOR, WillMoved.y, 50, Y_AXIS_SPEED,2);
            clock_t clk = clock();
            while (true) {
                if((clock()-clk) > AN_HOUR)
                {
                    //停止冲压 超时
                    MachineOp(MACHINE_STOP);
                    UpdateHistory("超时一个小时");
                    return;
                }

                if(PAUSE == (PAUSE&MachineState))
                {
                        lastCommand = PAUSE;
                        continue;
                }else if(STOP == (STOP&MachineState))
                {
                    lastCommand = STOP;
                    MachineOp(MACHINE_STOP);
                    UpdateHistory("按下了停止");
                    return;
                }else if(RUN == (RUN&MachineState))
                {
                    if(PAUSE == (PAUSE&lastCommand))
                    {
                        motion::GetIns()->CurrentCard()->absolute_move(X_AXIS_MOTOR, WillMoved.x, 50, X_AXIS_SPEED,2);
                        motion::GetIns()->CurrentCard()->absolute_move(Y_AXIS_MOTOR, WillMoved.y, 50, Y_AXIS_SPEED,2);
                    }
                    lastCommand = RUN;
                }
                else
                {
                    lastCommand = STOP;
                    MachineOp(MACHINE_STOP);
                    UpdateHistory("未知命令");
                    return;
                }


                int XDis = abs(static_cast<int>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,X_AXIS_MOTOR) - WillMoved.x));
                int YDis = abs(static_cast<int>(motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Y_AXIS_MOTOR) - WillMoved.y));

                if(XDis <=  abs(POS_ACCURCY) && YDis <=  abs(POS_ACCURCY))
                {
                    break;
                }
                Sleep(5);
            }

            //等待冲压
            if(!CheckPunchTimeOut())           {
                UpdateHistory("六秒内没有发现冲床运动，件请检查通气通电状态和感应器是否安装正确。");
                return;
            }
            show_Counter();

        }
        Points.pop_front();
    }
    motion::GetIns()->CurrentCard()->absolute_move(X_AXIS_MOTOR, Ctrl_Var.CurrentXAxisPostion, 50, X_AXIS_SPEED,2);
    clock_t clk_of_wait_X = clock();
    while(true){
        Sleep(3);
        if(PAUSE == (PAUSE&MachineState))
        {
                lastCommand = PAUSE;
                continue;
        }else if(STOP == (STOP&MachineState))
        {
            lastCommand = STOP;
            MachineOp(MACHINE_STOP);
            UpdateHistory("按下了停止");
            return;
        }else if(RUN == (RUN&MachineState))
        {
            if(PAUSE == (PAUSE&lastCommand))
            {
                motion::GetIns()->CurrentCard()->absolute_move(X_AXIS_MOTOR, Ctrl_Var.CurrentXAxisPostion, 50, X_AXIS_SPEED,2);
                Sleep(5);
            }
            if(true == isAxisStop(X_AXIS_MOTOR) || (clock() - clk_of_wait_X) > 20000){
                break;
            }
            lastCommand = RUN;
        }
        else
        {
            lastCommand = STOP;
            MachineOp(MACHINE_STOP);
            UpdateHistory("未知命令");
            return;
        }
    }
    MachineOp(MACHINE_STOP);
    UpdateHistory("停止冲压");
}


#ifdef OLDCODE
//同步相机和运动
void Mediator::Process()
{
    //参数准备
    try{
        Ctrl_Var.DealMethod = tagDealMethod::NORMAL;
        isSnapOver = false; evt_Punch.SetEvent(); evt_GetPoint.SetEvent();
        Sleep(32);
    if(!Excv::WaitValue(evt_Punch.State(),AN_HOUR) || !Excv::WaitValue(evt_GetPoint.State(),AN_HOUR)){
        UpdateMessage("超时了一个小时"); MachineOp(MACHINE_STOP);
    }

    //双模具下增加一些点
    if(1==DOUBLEMODEL && LINES_HORIZONTAL_FILL == LINE_METHOD)
    {
        //append        //找到最后一排点，并增加要冲的一个
        long long ModelHeight = cv::boundingRect(Ctrl_Var.ModelContours.at(0)).height + 2*Ctrl_Var.model_distance + 2*Ctrl_Var.margin_to_model;
        cv::Point pt;
        Excv::pix_to_pls(cv::Point(ModelHeight,ModelHeight),pt);

        if(AddOnePunch && !Ctrl_Var.ModelsPostion.empty()){
            AddOnePunch = false;
            std::vector<cv::Point2l> TalePoints = Ctrl_Var.ModelsPostion.back();
            for(auto &Pt : TalePoints){
                Pt.y += pt.y;
            }
            if(0!=pt.y){
            Ctrl_Var.ModelsPostion.push_back(TalePoints);
            char Msg[256] = {0};
            sprintf_s(Msg,"追加成功，新增%lld\n",ModelHeight);
            UpdateMessage(Msg);
            }
        }

    }




#ifdef NO_MOTION
  //  size_t Total = 0;
  //  for(auto v:Ctrl_Var.ModelsPostion) {Total += v.size();}
  //  printf(" 找到了%zd个点\n",Total);
#endif

    for (auto vpts3 : Ctrl_Var.Cal_ModelsPostion)
    {
        std::vector<cv::Point2l> vts;
        for (auto &pt : vpts3)
        {
            int x_ = pt.x* X_RATIO / X_DIS_PULS_RATIO;
            int y_ = pt.y* Y_RATIO / Y_DIS_PULS_RATIO;
            vts.push_back(cv::Point(x_, y_));
        }
        Ctrl_Var.ModelsPostion.push_back(vts);
    }

    //显示使用的，已经进行过偏移？
    DrawPts(Show_Image, Ctrl_Var, false);
    for (auto &vpts2 : Ctrl_Var.ModelsPostion) {
        for (auto &pt : vpts2)
        {
            //每个点进行偏移
            pt.y += Ctrl_Var.MovingForwardPuls;
        }
    }

    //对虚拟图像进行偏移
    if(LINES_HORIZONTAL_FILL == LINE_METHOD){
        //DLINES_HORIZONTAL_FILL
        int pix = 0;
        Excv::AxisYpls2pix(Ctrl_Var.MovingForwardPuls,pix);
        if(pix > 0) pix = -pix;
        cv::Mat t_mat = cv::Mat::zeros(2, 3, CV_32FC1);
        /*
        1   0   0
        0   1   bias
        */
        t_mat.at<float>(0, 0) = 1;
        t_mat.at<float>(0, 2) = 0; //水平平移量
        t_mat.at<float>(1, 1) = 1;
        t_mat.at<float>(1, 2) = pix; //竖直平移量
        cv::Mat src = Virtual_Image.clone();
        //图像偏移完毕
        cv::warpAffine(src, Virtual_Image, t_mat, Virtual_Image.size());
    }


    double rtHegiht = static_cast<double>(cv::boundingRect(Ctrl_Var.ModelContours.at(0)).height);
    double rtROI = CHECK_R2 - CHECK_R1;
    //最后一个点加上repeat高度就是末端，如果末端大于。。。针对小于2倍模具的
    if (rtROI < ROI_MODEL_RATIO * rtHegiht)
    {
        Ctrl_Var.Header_Pix_Ctrl = Ctrl_Var.Header_Pix;
        if(!Ctrl_Var.Cal_ModelsPostion.empty()){
            Ctrl_Var.DealMethod = tagDealMethod::FINDTALEPIX;
            Ctrl_Var.Tale_Pix_Ctrl = Ctrl_Var.Cal_ModelsPostion.back().at(0).y;
        }

    }
    Ctrl_Var.DealMethod_Ctrl = Ctrl_Var.DealMethod;

#ifdef NO_MOTION
    cv::Point pix;
    cv::Point pls(Ctrl_Var.MovingForwardPuls,Ctrl_Var.MovingForwardPuls);
    Excv::pls_to_pix(pls,pix);
    std::cout <<"前进了"<<Ctrl_Var.MovingForwardPuls<<"个脉冲"<< " 和 "<< pix.y<<"个像素"<<std::endl;
#endif
    HalconCpp::HObject Hobj;
    Excv::MatToHObj(Show_Image,Hobj);
    Excv::h_disp_obj(Hobj,MainWindowDispHd);
    }catch(cv::Exception ex)
    {
        MachineOp(MACHINE_STOP);
        UpdateMessage("Error In Process" + std::string(ex.what()));
    }
    catch(MException e)
    {
        MachineOp(MACHINE_STOP);
        UpdateMessage("Error In Process" + std::string(e.what()));
    }
    catch(std::out_of_range ex){
        MachineOp(MACHINE_STOP);
        UpdateMessage("Error In Process" + std::string(ex.what()));

    }
    //准备统计和显示
}

#endif
