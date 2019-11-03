#include "modelsetdialog.h"
#include "ui_modelsetdialog.h"
#include "mediator.h"
#include "global.h"

#include "excv.h"
#include <QMessageBox>
#include "global.h"
#include <process.h>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
//#include <QFileDialog>

bool isGrab = false;
bool isGrabing = false;

static void __stdcall DispMatImage_Ex(cv::Mat InputArray, void * pWnd = nullptr, bool Inv = false){
    try
    {
        HalconCpp::HObject hobj;
        Excv::MatToHObj(InputArray,hobj);
        Excv::h_disp_obj(hobj,Mediator::GetIns()->ModelDisp);
    }catch(cv::Exception ex)
    {
        throw std::exception(ex.what());
    }
}


void ModelSetDialog::CamSnap()
{
    printf("get image from camera\n");
#ifdef NO_MOTION
    QString fileName = QFileDialog::getOpenFileName(NULL,
        tr("Open Image"), "",
        tr("All Files (*)"));
    static cv::Mat Image;// = cv::imread(fileName.toLocal8Bit().data());
    Image = cv::imread(fileName.toLocal8Bit().data());
    if(Image.empty())   return;
    Snap_Image = Image.clone();
    CvCvtColor(Image,Snap_Image,CV_BGR2GRAY);
    cv::resize(Snap_Image, Image, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT), 0.0,0.0, cv::INTER_CUBIC);
    //cv::resize(Snap_Image, __g::GetIns()->Image, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT), 0.0,0.0, cv::INTER_CUBIC);
    MSerialsCamera::VoidImage() = Image.clone();
    try{
    HalconCpp::HObject Hobj,ROI;
    GenRectangle1(&ROI,CHECK_R1,CHECK_C1,CHECK_R2,CHECK_C2);
    Excv::MatToHObj(Snap_Image,Hobj);
    Excv::h_disp_obj(Hobj,Mediator::GetIns()->ModelDisp);
    SetDraw(Mediator::GetIns()->ModelDisp,"margin");
    SetColor(Mediator::GetIns()->ModelDisp,"yellow");
    DispObj(ROI,Mediator::GetIns()->ModelDisp);
    }catch(cv::Exception ex)
    {
        HalconCpp::SetTposition(Mediator::GetIns()->ModelDisp,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->ModelDisp,"red");
        HalconCpp::WriteString(Mediator::GetIns()->ModelDisp,ex.what());
    }
    catch(std::exception ex){
        HalconCpp::SetTposition(Mediator::GetIns()->ModelDisp,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->ModelDisp,"red");
        HalconCpp::WriteString(Mediator::GetIns()->ModelDisp,ex.what());
    }

#else
    //两张图片比较是否掉线

    try{
            MSerialsCamera::SnapTwice(Snap_Image);
    }catch(std::exception ex){
        QString fileName = QFileDialog::getOpenFileName(NULL,
            tr("Open Image"), "",
            tr("All Files (*)"));
        try{
        static cv::Mat Image;// = cv::imread(fileName.toLocal8Bit().data());
        Image = cv::imread(fileName.toLocal8Bit().data());
        if(Image.empty())   return;
        Snap_Image = Image.clone();
        CvCvtColor(Image,Snap_Image,CV_BGR2GRAY);
        cv::resize(Snap_Image, Image, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT), 0.0,0.0, cv::INTER_CUBIC);
        MSerialsCamera::VoidImage() = Image.clone();
        }catch(cv::Exception ex){
            std::cout << ex.what() <<std::endl;
            return;
        }
    }

    try{



    HalconCpp::HObject Hobj,ROI;
    GenRectangle1(&ROI,CHECK_R1,CHECK_C1,CHECK_R2,CHECK_C2);
    Excv::MatToHObj(Snap_Image,Hobj);
    Excv::h_disp_obj(Hobj,Mediator::GetIns()->ModelDisp);
    SetDraw(Mediator::GetIns()->ModelDisp,"margin");
    SetColor(Mediator::GetIns()->ModelDisp,"yellow");
    DispObj(ROI,Mediator::GetIns()->ModelDisp);
    }catch(cv::Exception ex)
    {
        HalconCpp::SetTposition(Mediator::GetIns()->ModelDisp,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->ModelDisp,"red");
        HalconCpp::WriteString(Mediator::GetIns()->ModelDisp,ex.what());
    }
    catch(std::exception ex){
        HalconCpp::SetTposition(Mediator::GetIns()->ModelDisp,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->ModelDisp,"red");
        HalconCpp::WriteString(Mediator::GetIns()->ModelDisp,ex.what());
    }
#endif
}

ModelSetDialog::ModelSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelSetDialog)
{
    ui->setupUi(this);
#ifdef SHOW_FULL_SCREEN
    showFullScreen();
#endif
    //isCabling = false;
    int nWndWidth = ui->widget->width();
    int nWndHeight= ui->widget->height();
    HalconCpp::SetCheck("~father");
    HalconCpp::OpenWindow(0, 0, nWndWidth, nWndHeight, (Hlong)ui->widget->winId(), "visible", "", &    Mediator::GetIns()->ModelDisp);
    HalconCpp::SetCheck("father");
    ui->pushButton_ModelDown->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_ModelDown->installEventFilter(this);
    ui->pushButton_ModelLeft->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_ModelLeft->installEventFilter(this);
    ui->pushButton_ModelRight->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_ModelRight->installEventFilter(this);
    ui->pushButton_ModelUp->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_ModelUp->installEventFilter(this);
    ui->pushButton_RotateAntiClock->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_RotateAntiClock->installEventFilter(this);
    ui->pushButton_RotateClock->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_RotateClock->installEventFilter(this);
    ui->pushButton_GammaMinor->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_GammaMinor->installEventFilter(this);
    ui->pushButton_GammaPlus->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_GammaPlus->installEventFilter(this);

    //bug??设计师的信号槽不能用了？
    connect(ui->pushButton_Snap,&QPushButton::clicked,[=](){CamSnap();});
    connect(ui->pushButton_OSK,&QPushButton::clicked,[=](){ QDesktopServices::openUrl(QUrl("osk.exe", QUrl::TolerantMode));});
    connect(ui->pushButton_CloseSet,&QPushButton::clicked,[=](){isGrab = false;close();});

    ui->lineEdit_Di->setText(QString::number(MODELD,'g',5));
    connect(ui->pushButton_GenCircle,&QPushButton::clicked,[=](){on_pushButton_GenCircle_clicked();});

    ui->lineEdit_RectW->setText(QString::number(MODELW,'g',5));
    ui->lineEdit_RectH->setText(QString::number(MODELH,'g',5));
    connect(ui->pushButton_GenRect,&QPushButton::clicked,[=](){on_pushButton_GenRect_clicked();});

    connect(ui->pushButton_GetModel,&QPushButton::clicked,[=](){QString fileName = QFileDialog::getOpenFileName(NULL,tr("Open Image"), MODEL,tr("All Files (*)"));
        Mediator::GetIns()->Load_Model(fileName.toLocal8Bit().data(),Mediator::GetIns()->ModelDisp,true);
    });
    connect(ui->pushButton_SetCheckArea,&QPushButton::clicked,[=](){
        using namespace HalconCpp;
        try{
        if(isDraw) return;
        HObject ROI;
        Snap_Image = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE);        HalconCpp::HObject Hobj;
        DispMatImage_Ex(Snap_Image);
        SetColor(Mediator::GetIns()->ModelDisp,"yellow");
        SetDraw(Mediator::GetIns()->ModelDisp,"margin");
        isDraw = true;
        HTuple R1,R2,C1,C2;
        DrawRectangle1(Mediator::GetIns()->ModelDisp, &R1,&C1,&R2,&C2);
        CHECK_R1 = R1.D();CHECK_C1 = C1.D();CHECK_R2 = R2.D();CHECK_C2 = C2.D();
        GenRectangle1(&ROI,CHECK_R1,CHECK_C1,CHECK_R2,CHECK_C2);
        DispObj(ROI,Mediator::GetIns()->ModelDisp);
        isDraw = false;
        Preference::GetIns()->prj->WriteSettings();
        }
        catch (HalconCpp::HException except)
        {
            HalconCpp::HTuple Except;
            except.ToHTuple(&Except);
            return;
        }
        catch(cv::Exception ex)
        {
            printf("ex %s\n",ex.what());
            return;
        }
    });

    connect(ui->pushButton_DrawModelArea,&QPushButton::clicked,[=](){
        using namespace HalconCpp;
        try{
        if(isDraw) return;
        HObject ROI;
        DispMatImage_Ex(Snap_Image);
        SetColor(Mediator::GetIns()->ModelDisp,"cyan");
        SetDraw(Mediator::GetIns()->ModelDisp,"margin");
        isDraw = true;
        HTuple R1,R2,C1,C2;
        DrawRectangle1(Mediator::GetIns()->ModelDisp, &R1,&C1,&R2,&C2);
        MODEL_R1 = R1.D();MODEL_C1 = C1.D();MODEL_R2 = R2.D();MODEL_C2 = C2.D();
        GenRectangle1(&ROI,R1,C1,R2,C2);
        DispObj(ROI,Mediator::GetIns()->ModelDisp);
        isDraw = false;
        Preference::GetIns()->prj->WriteSettings();
        }
        catch (HalconCpp::HException except)
        {
            HTuple ErrorInfo;
            except.ToHTuple(&ErrorInfo);
            isDraw = false;
            return;
        }
        catch(cv::Exception ex)
        {
            isDraw = false;
            HalconCpp::SetColor(Mediator::GetIns()->ModelDisp,"red");
            HalconCpp::SetTposition(Mediator::GetIns()->ModelDisp,1,1);
            HalconCpp::WriteString(Mediator::GetIns()->ModelDisp,ex.what());
        }
    });

     connect(ui->pushButton_SetModel,&QPushButton::clicked,[=](){
         try{

                HalconCpp::HObject Hobj,ROI;
                if(Snap_Image.empty()) return;
                DispMatImage_Ex(Snap_Image);
                Excv::MatToHObj(Snap_Image,Hobj);
                cv::Rect Rt(MODEL_C1,MODEL_R1,MODEL_C2-MODEL_C1,MODEL_R2-MODEL_R1);
                cv::Mat ImageReduced,thresHold;
                Snap_Image(Rt).copyTo(ImageReduced);
                cv::threshold(ImageReduced,thresHold,Get_Obj_Threshold,255,CV_THRESH_BINARY);
                std::string model_name = Excv::cv_write_image(thresHold, MODEL, "Model",true);
                Mediator::GetIns()->Load_Model(model_name,Mediator::GetIns()->ModelDisp,true);
         }
         catch(cv::Exception ex)
         {
               HalconCpp::SetColor(Mediator::GetIns()->ModelDisp,"red");
               HalconCpp::SetTposition(Mediator::GetIns()->ModelDisp,1,1);
               HalconCpp::WriteString(Mediator::GetIns()->ModelDisp,ex.what());
         }
     });

     connect(ui->pushButton_Grab,&QPushButton::pressed,[=](){on_pushButton_Grab_clicked();});

     isOpend = true;
     for(int i =  LINES_HORIZONTAL ; i <= LINES_HORIZONTAL_USER	 ; i++)
     ui->comboBox_LineMethod->addItem(QString::fromLocal8Bit(_global::LineMethod()[i].c_str()));
     ui->comboBox_LineMethod->setCurrentIndex(LINE_METHOD);
     connect(ui->comboBox_LineMethod,&QComboBox::currentTextChanged,[=](QString Str){
         int Idx = ui->comboBox_LineMethod->currentIndex();
         LINE_METHOD = Idx;
         Preference::GetIns()->prj->WriteSettings(-1);
         std::cout << Str.toStdString().data() << "and Index is " << LINE_METHOD<< std::endl;
     });

    // Doubleel
     for(int i = 0;i<3;i++){
        ui->comboBox_DoubleModel->addItem(QString::fromLocal8Bit(_global::DoubleModel()[i].c_str()));
     }

    ui->comboBox_DoubleModel->setCurrentIndex(DOUBLEMODEL);
    connect(ui->comboBox_DoubleModel,&QComboBox::currentTextChanged,[=](QString Str){
        int Idx = ui->comboBox_DoubleModel->currentIndex();
        DOUBLEMODEL = Idx;
        Preference::GetIns()->prj->WriteSettings(-1);

    });

     for(int i =  0 ; i <= 5 ; i++)
     ui->comboBox_CameraRes->addItem(QString::fromLocal8Bit(_global::CameraResolution()[i].c_str()));
     ui->comboBox_CameraRes->setCurrentIndex(MODE_INDEX);
     connect(ui->comboBox_CameraRes,&QComboBox::currentTextChanged,[=](QString Str){
         int Idx = ui->comboBox_CameraRes->currentIndex();
         MODE_INDEX = Idx;

         int Camera_qty = MSerialsCamera::init_camera(MODE_INDEX);
         if(Camera_qty < 1){std::cout << "没有发现相机...";}
         else
         {
         Preference::GetIns()->prj->WriteSettings(-1);
         }
         std::cout << Str.toStdString().data() << "and Index is " << LINE_METHOD<< std::endl;
     });


    ui->lineEdit_Gamma->setText(QString::number(CAMERAGAMMA,'g',4));


  //  DoubleModel


     //启动定时器
     QTimer *timer_io = new QTimer();
     //设置定时器每个多少毫秒发送一个timeout()信号
     timer_io->setInterval(20);
     connect(timer_io, &QTimer::timeout, [=]() {this->OnTimer();});
     timer_io->start();
   ui->pushButton_SaveImage->setVisible(false);
//
}

void ModelSetDialog::TransRegion(bool isPaint)
{
    try{
    HObject ho_DestRegions_,ho_Mirror;
    ho_Mirror = ho_DestRegions;
    AreaCenter(ho_DestRegions,&hv_Area,&hv_Row,&hv_Column);
    if(MirrorX) MirrorRegion(ho_DestRegions,&ho_Mirror,"column",2*hv_Column);
    if(MirrorY) MirrorRegion(ho_Mirror,&ho_Mirror,"row",2*hv_Row);
    HalconCpp::SetCheck("~give_error");
    HalconCpp::SetColor(Mediator::GetIns()->ModelDisp,"cyan");
    HalconCpp::SetDraw(Mediator::GetIns()->ModelDisp,"fill");
    MoveRegion(ho_Mirror, &ho_DestRegions_, hv_Row_Moved, hv_Column_Moved);
    HTuple Str = "已经旋转：";
    AreaCenter(ho_DestRegions_,&hv_Area,&hv_Row,&hv_Column);
    HomMat2dIdentity(&hv_HomMat2DIdentity);
    HomMat2dRotate(hv_HomMat2DIdentity, hv_phi.TupleRad(), hv_Row, hv_Column,&hv_HomMat2DRotate);
    //Apply an arbitrary affine transformation to an image
    AffineTransRegion(ho_DestRegions_, &ho_DestRegions_, hv_HomMat2DRotate, "constant");
    SetTposition(Mediator::GetIns()->ModelDisp, 1, 1);
    Excv::h_disp_obj(Mediator::GetIns()->HalconImage,Mediator::GetIns()->ModelDisp);
    HalconCpp::DispObj(ho_DestRegions_,Mediator::GetIns()->ModelDisp);
    WriteString(Mediator::GetIns()->ModelDisp,Str+hv_phi.ToString());

    if(isPaint)
    {
        HTuple r1 = 0,c1 = 0,r2 = 0,c2 = 0,Area;
        HObject ImageResult,Image = Mediator::GetIns()->HalconImage;
        PaintRegion(ho_DestRegions_,Image,&ImageResult,255,"fill");
        AreaCenter(ho_DestRegions,&Area,&r1,&c1);
        AreaCenter(ho_DestRegions_,&Area,&r2,&c2);
        WriteImage(ImageResult,"bmp",255,"TMP");
        cv::Mat Model,Load = cv::imread("TMP.bmp");
        CvCvtColor(Load,Model,CV_BGR2GRAY);
        int r11 = r1[0].D();
        int c11 = c1[0].D();
        int r22 = r2[0].D();
        int c22 = c2[0].D();
        cv::line(Model,cv::Point(c11,r11),cv::Point(c22,r22),cv::Scalar(255,128,0),2);
      //  char model_name[256] = { 0 };
      //  sprintf_s(model_name, "宽%5.2fmm-高%5.2fmm",w,h);
        std::string _model_name = Excv::cv_write_image(Model, "Model", "Copy-",true);
        Mediator::GetIns()->Load_Model(_model_name,Mediator::GetIns()->ModelDisp);
    }

    }catch(HalconCpp::HException except)
    {
        HTuple ExceptionMessage;
        except.ToHTuple(&ExceptionMessage);
        SetTposition(Mediator::GetIns()->ModelDisp, 20, 21);
        WriteString(Mediator::GetIns()->ModelDisp, ExceptionMessage);
    }
    catch(cv::Exception ex)
    {

    }

}

void ModelSetDialog::OnTimer()
{
    static int DelayTime = 25;
    using namespace HalconCpp;
    switch(Operation)
    {
    case NOP:DelayTime = 25;break;
    case MIRROR_UPDOWN:
        break;
    case MIRROR_LEFTRIGHT:break;
    case ROTATE_CLOCK:
        if(25 == DelayTime)
        {
            DelayTime--;
        }
        else if(DelayTime > 0)
        {
            DelayTime--;
            break;
        }

        hv_phi = hv_phi + 1;
        TransRegion();
        break;
    case ROTATE_ANTICLOCK:
        if(25 == DelayTime)
        {
            DelayTime--;
        }
        else if(DelayTime > 0)
        {
            DelayTime--;
            break;
        }

        hv_phi = hv_phi - 1;
        TransRegion();
        break;
    case MODEL_MOVE_UP:
        if(25 == DelayTime)
        {
            DelayTime--;
        }
        else if(DelayTime > 0)
        {
            DelayTime--;
            break;
        }
        hv_Row_Moved = hv_Row_Moved - 1;
        TransRegion();
        break;
    case MODEL_MOVE_DOWN:
        if(25 == DelayTime)
        {
            DelayTime--;
        }
        else if(DelayTime > 0)
        {
            DelayTime--;
            break;
        }
        hv_Row_Moved = hv_Row_Moved + 1;
        TransRegion();
        break;
    case MODEL_MOVE_LEFT:
        if(25 == DelayTime)
        {
            DelayTime--;
        }
        else if(DelayTime > 0)
        {
            DelayTime--;
            break;
        }
        hv_Column_Moved = hv_Column_Moved - 1;
        TransRegion();
        break;
    case MODEL_MOVE_RIGHT:
        if(25 == DelayTime)
        {
            DelayTime--;
        }
        else if(DelayTime > 0)
        {
            DelayTime--;
            break;
        }
        hv_Column_Moved = hv_Column_Moved + 1;
        TransRegion();
        break;
    case GAMMAMINUS:
        if(25 == DelayTime)
        {
            DelayTime--;
        }
        else if(DelayTime > 0)
        {
            DelayTime--;
            break;
        }
        if(CAMERAGAMMA > 0.01)
        CAMERAGAMMA = CAMERAGAMMA - 0.01;
        ui->lineEdit_Gamma->setText(QString::number(CAMERAGAMMA,'g',4));
        set_gamma(CAMERAGAMMA);
        Preference::GetIns()->prj->WriteSettings(-1);

        break;
    case GAMMAPLUS:
        if(25 == DelayTime)
        {
            DelayTime--;
        }
        else if(DelayTime > 0)
        {
            DelayTime--;
            break;
        }
        if(CAMERAGAMMA < 2.01)
        CAMERAGAMMA = CAMERAGAMMA + 0.01;
        ui->lineEdit_Gamma->setText(QString::number(CAMERAGAMMA,'g',4));
        set_gamma(CAMERAGAMMA);
        Preference::GetIns()->prj->WriteSettings(-1);

        break;

    default :break;
    }
}

bool ModelSetDialog::eventFilter(QObject* obj,QEvent *event)
{
    static bool isTouched = false;
    switch (event->type()) {
    case QEvent::TouchBegin:
        isTouched = true;
        if(obj == ui->pushButton_ModelDown)
        {
            Operation = MODEL_MOVE_DOWN;
        }
        else if(obj == ui->pushButton_ModelLeft)
        {
            Operation = MODEL_MOVE_LEFT;
        }
        else if(obj == ui->pushButton_ModelRight)
        {
            Operation = MODEL_MOVE_RIGHT;
        }
        else if(obj == ui->pushButton_ModelUp)
        {
            Operation = MODEL_MOVE_UP;
        }
        else if(obj == ui->pushButton_RotateAntiClock)
        {
            Operation = ROTATE_ANTICLOCK;
        }
        else if(obj == ui->pushButton_RotateClock)
        {
            Operation = ROTATE_CLOCK;
        }
        else if(obj == ui->pushButton_GammaMinor)
        {
            Operation = GAMMAMINUS;
        }
        else if(obj == ui->pushButton_GammaPlus)
        {
            Operation = GAMMAPLUS;
        }

        break;
    case QEvent::MouseButtonPress:
        if(isTouched) break;
        if(obj == ui->pushButton_ModelDown)
        {
            Operation = MODEL_MOVE_DOWN;
        }
        else if(obj == ui->pushButton_ModelLeft)
        {
            Operation = MODEL_MOVE_LEFT;
        }
        else if(obj == ui->pushButton_ModelRight)
        {
            Operation = MODEL_MOVE_RIGHT;
        }
        else if(obj == ui->pushButton_ModelUp)
        {
            Operation = MODEL_MOVE_UP;
        }
        else if(obj == ui->pushButton_RotateAntiClock)
        {
            Operation = ROTATE_ANTICLOCK;
        }
        else if(obj == ui->pushButton_RotateClock)
        {
            Operation = ROTATE_CLOCK;
        }
        else if(obj == ui->pushButton_GammaMinor)
        {
            Operation = GAMMAMINUS;
        }
        else if(obj == ui->pushButton_GammaPlus)
        {
            Operation = GAMMAPLUS;
        }

        break;
    case QEvent::TouchEnd:
    case QEvent::MouseButtonRelease:
        if(obj == ui->pushButton_MirrorLR)
        {
            Operation = NOP;
        }
        else if(obj == ui->pushButton_MirrorUD)
        {
            Operation = NOP;
        }
        else if(obj == ui->pushButton_ModelDown)
        {
            Operation = NOP;
        }
        else if(obj == ui->pushButton_ModelLeft)
        {
            Operation = NOP;
        }
        else if(obj == ui->pushButton_ModelRight)
        {
            Operation = NOP;
        }
        else if(obj == ui->pushButton_ModelUp)
        {
            Operation = NOP;
        }
        else if(obj == ui->pushButton_RotateAntiClock)
        {
            Operation = NOP;
        }
        else if(obj == ui->pushButton_RotateClock)
        {
            Operation = NOP;
        }
        else if(obj == ui->pushButton_GammaMinor)
        {
            Operation = NOP;
        }
        else if(obj == ui->pushButton_GammaPlus)
        {
            Operation = NOP;
        }

        isTouched = false;
        break;
    default:
        break;
    }

    return QDialog::eventFilter(obj,event);
}


ModelSetDialog::~ModelSetDialog()
{
    delete ui;
}



void ModelSetDialog::on_pushButton_clicked()
{
    close();
}

bool ModelSetDialog::Load_Model(std::string file_name)
{
    file_name = "cpp";
  return true;
}

void ModelSetDialog::on_pushButton_GenCircle_clicked()
{
    double Diameter = ui->lineEdit_Di->text().toDouble();
    double radius = Diameter/2;
    if (radius < 1)
    {
        QMessageBox::information(NULL,"Error",QString::fromLocal8Bit("尺寸太小，请检查输入是否正确？"));
        return;
    }
    cv::Point pix;
    Excv::mm_to_pix(cv::Point2d(2*radius, 2*radius), pix);
    int image_h = static_cast<int>(pix.x);
    int image_w = image_h;
    cv::Mat Model(image_h+3, image_w+3, CV_8UC1, cv::Scalar(0));
    cv::Point center(image_w / 2+1, image_h / 2+1);
    cv::circle(Model, center, static_cast<int>(image_w / 2),cv::Scalar(255),-1);
    char model_name[256] = { 0 };
    sprintf(model_name, "直径_%5.2fmm", Diameter);
    std::string _model_name = Excv::cv_write_image(Model, MODEL, model_name);
    Mediator::GetIns()->Load_Model(_model_name,Mediator::GetIns()->ModelDisp);
    MODELD = Diameter;
    Preference::GetIns()->prj->WriteSettings();

}

void ModelSetDialog::on_pushButton_GenRect_clicked()
{
    double w = ui->lineEdit_RectW->text().toDouble();
    double h = ui->lineEdit_RectH->text().toDouble();
    if (w < 2 || h<2)
    {
        QMessageBox::information(NULL,"Error",QString::fromLocal8Bit("尺寸太小，请检查输入是否正确？"));
        return;
    }
    cv::Point pix;
    Excv::mm_to_pix(cv::Point2d(w, h), pix);
    int image_w = static_cast<int>(pix.x);
    int image_h = static_cast<int>(pix.y);
    cv::Rect Rt(1,1,image_w,image_h);
    cv::Mat Model(image_h+3, image_w+3, CV_8UC1, cv::Scalar(0));
    cv::rectangle(Model,Rt,cv::Scalar(255,255,255),-1);
    char model_name[256] = { 0 };
    sprintf_s(model_name, "宽%5.2fmm-高%5.2fmm",w,h);
    std::string _model_name = Excv::cv_write_image(Model, MODEL, model_name);
    Mediator::GetIns()->Load_Model(_model_name,Mediator::GetIns()->ModelDisp);

    MODELW = w;
    MODELH = h;
    Preference::GetIns()->prj->WriteSettings();
    ui->pushButton_SaveImage->setVisible(false);
}



void ModelSetDialog::on_pushButton_SaveImage_clicked()
{
    try{
        cv::imwrite("a.png",Snap_Image);
        cv::Mat Gray;
        MSerialsCamera::CvtColor(Snap_Image,Gray);
        cv::imwrite("b.png",Gray);
    }catch(cv::Exception ex)
    {

    }
}

unsigned int __stdcall Grab(void* p)
{
    if(isGrabing) return 1;
    isGrabing = true;
    for(;isGrab;)
    {
        cv::Mat Snap = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
        DispMatImage_Ex(Snap);
    }
    isGrabing = false;
    return 0;
}
void ModelSetDialog::on_pushButton_Grab_clicked()
{
    if(isGrab == true)
    {
       isGrab = false;
       isDraw = false;
       ui->pushButton_Grab->setText(QString::fromLocal8Bit("连续截图"));
       printf("is grab\n");
    }
    else
    {
       if(isGrabing)
       {
            return;
       }
       isGrab = true;
       isDraw = true;
       printf("is not grab\n");
       ui->pushButton_Grab->setText(QString::fromLocal8Bit("停止截图"));
       (HANDLE)_beginthreadex(NULL, 0, Grab, this, 0, NULL);
    }
}






void ModelSetDialog::on_pushButton_OSK_clicked()
{

}

void ModelSetDialog::on_pushButton_CopyStart_clicked()
{
    try{
        HalconCpp::SetCheck("~give_error");
        HalconCpp::SetColor(Mediator::GetIns()->ModelDisp,"yellow");
         HalconCpp::SetDraw(Mediator::GetIns()->ModelDisp,"fill");
         Excv::h_disp_obj(Mediator::GetIns()->HalconImage,Mediator::GetIns()->ModelDisp);
        Threshold(Mediator::GetIns()->HalconImage, &ho_binImage, 128, 255);
        Connection(ho_binImage, &ho_Connections);
        SelectShapeStd(ho_Connections, &ho_DestRegions, "max_area", 0.6);
        AreaCenter(ho_DestRegions, &hv_Area, &hv_Row, &hv_Column);
        HalconCpp::DispObj(ho_DestRegions,Mediator::GetIns()->ModelDisp);
        hv_phi = 0;
        hv_Column_Moved = hv_Row_Moved = 0;
        MirrorX = false; MirrorY = false;
    }catch(HalconCpp::HException except)
    {
        HTuple ExceptionMessage;
        except.ToHTuple(&ExceptionMessage);
        SetTposition(Mediator::GetIns()->ModelDisp, 20, 21);
        WriteString(Mediator::GetIns()->ModelDisp, ExceptionMessage);
    }
}

void ModelSetDialog::on_pushButton_MirrorLR_clicked()
{
    if(MirrorX) MirrorX = false;
    else MirrorX = true;
    TransRegion();
}

void ModelSetDialog::on_pushButton_MirrorUD_clicked()
{
    if(MirrorY) MirrorY = false;
    else MirrorY = true;
    TransRegion();
}

void ModelSetDialog::on_pushButton_SaveImage_2_clicked()
{
    TransRegion(true);
}


