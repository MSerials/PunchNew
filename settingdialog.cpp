#include "settingdialog.h"
#include "ui_settingdialog.h"
#include "QProcess"
#include <QDesktopServices>
#include <QUrl>
#include <mediator.h>
#include "HalconCpp.h"
#include "global.h"
#include "excv.h"

#ifdef Q_OS_WIN32
#include "Windows.h"
#include <ShellAPI.h>

#endif

void __stdcall DispMatImage_Ex(cv::Mat InputArray, void * pWnd, bool Inv){
    try
    {
        HalconCpp::HObject hobj;
        Excv::MatToHObj(InputArray,hobj);
        Excv::h_disp_obj(hobj,Mediator::GetIns()->SetDispHd);
    }catch(cv::Exception ex)
    {

    }
}

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
#ifdef SHOW_FULL_SCREEN
    showFullScreen();
#endif

    ui->pushButton_Left_Limit->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_Left_Limit->installEventFilter(this);

    ui->pushButton_LeftLimit2->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_LeftLimit2->installEventFilter(this);

    ui->pushButton_RIGHT_2->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_RIGHT_2->installEventFilter(this);

    ui->pushButton_LEFT_2->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_LEFT_2->installEventFilter(this);

    ui->pushButton_FWD_2->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_FWD_2->installEventFilter(this);

    ui->pushButton_BACK_2->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_BACK_2->installEventFilter(this);

    int nWndWidth = ui->widget->width();
    int nWndHeight= ui->widget->height();
    HalconCpp::SetCheck("~father");
    HalconCpp::OpenWindow(0, 0, nWndWidth, nWndHeight, (Hlong)ui->widget->winId(), "visible", "", &Mediator::GetIns()->SetDispHd);
    HalconCpp::SetCheck("father");

//初始化界面文字
ui->lineEdit_MoveDistance->setText(QString::number(100));

ui->lineEdit_XAxisDisPerPls->setText(QString::number(X_DIS_PULS_RATIO,'g',8));

ui->lineEdit_YAxisDisPerPls->setText(QString::number(Y_DIS_PULS_RATIO,'g',8));

ui->lineEdit_XAxisCamDis->setText(QString::number(X_CAM_DISTANCE_PLS));

ui->lineEdit_YAxisCamDis->setText(QString::number(Y_CAM_DISTANCE_PLS_EX));

cv::Point pls(X_CAM_DISTANCE_PLS,Y_CAM_DISTANCE_PLS_EX);
cv::Point2d mm(0,0);
Excv::pls_to_mm(pls,mm);
ui->label_cam_to_PunchX->setText(QString::fromLocal8Bit("相机冲床X轴距离：") + QString::number(mm.x,'g',6) + QString::fromStdString(" mm"));
ui->label_cam_to_PunchY->setText(QString::fromLocal8Bit("相机冲床Y轴距离：") + QString::number(mm.y,'g',6) + QString::fromStdString(" mm"));

ui->lineEdit_XAxisSpeed->setText(QString::number(X_AXIS_SPEED));

ui->lineEdit_YAxisSpeed->setText(QString::number(Y_AXIS_SPEED));

ui->lineEdit_BoardModelDistance->setText(QString::number(static_cast<int>(BOARD_DISTANCE)));

ui->lineEdit_ModelGap->setText(QString::number(static_cast<int>(MODEL_GAP)));

cv::Point2d mmDis(0,0);
cv::Point plsDis(POS_ACCURCY,POS_ACCURCYY);
Excv::pls_to_mm(plsDis,mmDis);
ui->lineEdit_DistanceLeftToPunch->setText(QString::number(mmDis.x));
ui->lineEdit_DistanceLeftToPunchY->setText(QString::number(mmDis.y));

ui->lineEdit_CamAngle->setText(QString::number(CAMERA_ANGLE,'g',8));

ui->lineEdit_XLimit->setText(QString::number(X_AXIS_LIMIT));

ui->lineEdit_CabliBoardSize->setText(QString::number(239.83));

ui->lineEdit_CamXDisPerPix->setText(QString::number(X_RATIO,'g',6));

ui->lineEdit_CamYDisPerPix->setText(QString::number(Y_RATIO,'g',6));

ui->lineEdit_LineSkip->setText(QString::number(VERTICAL_SKIP_RATIO,'g',5));

ui->QLineEdit_ReturnRatio->setText(QString::number(RETURNRATIO,'g',5));

ui->QLineEdit_SnapDelay->setText(QString::number(SNAP_DELAY));

isOpened = true;


//ui->pushButton_SaveImage->setVisibale(false);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::MachineOp(int Sel, int SecondSel)
{
    printf("operate %d\n",Sel);
}


void SettingDialog::on_pushButton_KeyBoard_clicked()
{
    QDesktopServices::openUrl(QUrl("osk.exe", QUrl::TolerantMode));
  //  QProcess::startDetached(QString::fromLocal8Bit("osk.exe"));
   // system("osk.exe");
}

void SettingDialog::on_pushButton_CloseSet_clicked()
{
    close();
}

void SettingDialog::on_pushButton_SavePara_clicked()
{
    //初始化界面文字

    X_DIS_PULS_RATIO = ui->lineEdit_XAxisDisPerPls->text().toDouble();

    Y_DIS_PULS_RATIO = ui->lineEdit_YAxisDisPerPls->text().toDouble();

    X_CAM_DISTANCE_PLS = ui->lineEdit_XAxisCamDis->text().toInt();

    Preference::GetIns()->prj->yAxis_cam_distance_pls = ui->lineEdit_YAxisCamDis->text().toInt();

    cv::Point pls(X_CAM_DISTANCE_PLS,Y_CAM_DISTANCE_PLS_EX);
    cv::Point2d mm(0,0);
    Excv::pls_to_mm(pls,mm);
    ui->label_cam_to_PunchX->setText(QString::fromLocal8Bit("相机冲床X轴距离：") + QString::number(mm.x,'g',6) + QString::fromStdString(" mm"));
    ui->label_cam_to_PunchY->setText(QString::fromLocal8Bit("相机冲床Y轴距离：") + QString::number(mm.y,'g',6) + QString::fromStdString(" mm"));

    X_AXIS_SPEED = ui->lineEdit_XAxisSpeed->text().toInt();

    Y_AXIS_SPEED = ui->lineEdit_YAxisSpeed->text().toInt();

    BOARD_DISTANCE = ui->lineEdit_BoardModelDistance->text().toDouble();

    MODEL_GAP = ui->lineEdit_ModelGap->text().toDouble();


    double mm_dis = ui->lineEdit_DistanceLeftToPunch->text().toDouble();
    double mm_disY = ui->lineEdit_DistanceLeftToPunchY->text().toDouble();

    cv::Point2d mmDis(mm_dis,mm_disY);
    cv::Point plsDis(0,0);
    Excv::mm_to_pls(mmDis,plsDis);
    POS_ACCURCY = plsDis.x;
    POS_ACCURCYY = plsDis.y;

    CAMERA_ANGLE = ui->lineEdit_CamAngle->text().toDouble();

    X_AXIS_LIMIT = ui->lineEdit_XLimit->text().toInt();

    if(0.05>ui->lineEdit_LineSkip->text().toDouble())
    {
        ui->lineEdit_LineSkip->setText(QString::number(0.05,'g',5));
    }
    VERTICAL_SKIP_RATIO = ui->lineEdit_LineSkip->text().toDouble();

    RETURNRATIO = ui->QLineEdit_ReturnRatio->text().toDouble();

    SNAP_DELAY = ui->QLineEdit_SnapDelay->text().toInt();

    Preference::GetIns()->prj->WriteSettings();
}

void SettingDialog::on_pushButton_CabliSize_clicked()
{
    double BoardSize = ui->lineEdit_CabliBoardSize->text().toDouble();
    double x_ = ui->lineEdit_CamXDisPerPix->text().toDouble();
    double y_ = ui->lineEdit_CamYDisPerPix->text().toDouble();
    //两张图片比较是否掉线
    clock_t clk = clock();
    cv::Mat Image_Try        = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
    cv::Mat Snap_Image       = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
    std::cout <<"拍摄耗时 "<<clock()-clk<<std::endl;
    if(true == MSerialsCamera::isEqual(Snap_Image,Image_Try))
    {
        for (int i = 0;;i++)
        {
            Sleep(100);
            if(MSerialsCamera::init_camera() > 0) {
                    Snap_Image       = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
                    break;
            }
            else if (i>3)
            {
                QString fileName = QFileDialog::getOpenFileName(NULL,
                    tr("Open Image"), "",
                    tr("Image File (*.jpg)||Image File (*.bmp)||All Files (*)"));
                cv::Mat Image = cv::imread(fileName.toStdString());
                if(Image.empty())   return;
                Snap_Image = Image.clone();
                MSerialsCamera::CvtColor(Snap_Image,Snap_Image);
                break;
            }
        }
    }
    try{
        double square_size = -1;
        Control_Var Ctrl_Var;
        if(Snap_Image.empty())  return;
        Ctrl_Var.DispMatImage = DispMatImage_Ex;
        square_size = CvSquareSize(Snap_Image, Ctrl_Var);
        if(square_size < 0)
        {
            HalconCpp::SetTposition(Mediator::GetIns()->SetDispHd,10,0);
            HalconCpp::SetColor(Mediator::GetIns()->SetDispHd,"red");
            HalconCpp::WriteString(Mediator::GetIns()->SetDispHd,"没有找到标定板");
            return;
        }
        double x = -1;
        double y = -1;
        square_size = BoardSize/square_size;
        if(x_ > 0) x  = (x_+square_size)/2.0; else x = square_size;
        if(y_>0) y = (y_+square_size)/2.0; else y=square_size;

        static std::vector<double> vecX;
        static std::vector<double> vecY;
        vecX.push_back(x);
        vecY.push_back(y);
        double totalX = 0;
        for(auto &_x: vecX){totalX += _x;}
        x = totalX/static_cast<double>(vecX.size());
        double totalY = 0;
        for(auto &_y: vecY){totalY += _y;}
        y = totalY/static_cast<double>(vecY.size());

        x = 0.998*x;
        y = 0.998*y;

        X_RATIO = x;
        Y_RATIO = y;
        static double MaxV = -1;
        static double MinV = 999999999999;
        if(x>MaxV) MaxV = x;
        if(x<MinV) MinV = x;
        std::cout << "整个冲床可能产生的误差为 "<<1024*(MaxV-MinV)<<std::endl;
        Preference::GetIns()->prj->WriteSettings();
        ui->lineEdit_CamXDisPerPix->setText(QString::number(x,'g'));
        ui->lineEdit_CamYDisPerPix->setText(QString::number(y,'g'));
    }catch(cv::Exception ex)
    {
        DispMatImage_Ex(Snap_Image,nullptr,false);
        HalconCpp::SetTposition(Mediator::GetIns()->SetDispHd,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->SetDispHd,"red");
        HalconCpp::WriteString(Mediator::GetIns()->SetDispHd,ex.what());
    }
}

unsigned int __stdcall CabliCamera(void * pThis)
{
    static bool isCabling = false;
    if(isCabling) return 1;
    isCabling = true;
    Control_Var Ctrl_Var;
    Ctrl_Var.DispMatImage = DispMatImage_Ex;
    try {
        std::string error_s;
        printf("平均格子长度 %f\n", CvCabli(334, Ctrl_Var, error_s, 60,CABLI));
        if (strlen(error_s.c_str()) > 0)
        {
            QMessageBox::warning(NULL,QString("Error"),QString(error_s.c_str()).toLocal8Bit());
        }
        else
        {
            HalconCpp::SetTposition(Mediator::GetIns()->SetDispHd,10,0);
            HalconCpp::SetColor(Mediator::GetIns()->SetDispHd,"green");
            HalconCpp::WriteString(Mediator::GetIns()->SetDispHd,"标定完毕,软件重启后载入标定数据");
        }
    }catch(cv::Exception ex)
    {
        HalconCpp::SetTposition(Mediator::GetIns()->SetDispHd,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->SetDispHd,"red");
        HalconCpp::WriteString(Mediator::GetIns()->SetDispHd,ex.what());
    }
    isCabling = false;
    return 0;
}

void SettingDialog::on_pushButton_Cabli_clicked()
{
    (HANDLE)_beginthreadex(NULL, 0, CabliCamera, this, 0, NULL);
}



void SettingDialog::on_pushButton_SaveAsCalbiImage_clicked()
{
    static bool isSaving = false;
    if(isSaving) return;
    isSaving = true;
    try{
    cv::Mat ColorView, Snap_Image;
    Snap_Image = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_ORIGIN,0.0);
    MSerialsCamera::CvtColor(Snap_Image,ColorView,CV_GRAY2BGR);
    if(Excv::CheckCabliImage(Snap_Image,ColorView))
    {
        Excv::cv_write_image(Snap_Image,CABLI,"Cabli",true);
    }
    HalconCpp::HObject Hobj;
    Excv::MatToHObj(ColorView,Hobj);
    Excv::h_disp_obj(Hobj,Mediator::GetIns()->SetDispHd);
    }catch(cv::Exception ex)
    {
        HalconCpp::SetTposition(Mediator::GetIns()->SetDispHd,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->SetDispHd,"red");
        HalconCpp::WriteString(Mediator::GetIns()->SetDispHd,ex.what());
    }
    isSaving = false;
}

void SettingDialog::on_pushButton_Cabli_File_clicked()
{
    QDesktopServices::openUrl(QUrl(CABLI, QUrl::TolerantMode));
   // system("explorer.exe Cabli");
}

void SettingDialog::on_pushButton_ChangeAdjust_clicked()
{
    switch(AdjustSel)
    {
    case MOVE_:
        AdjustSel = ADJUST1;
        ui->pushButton_FWD_2->setText(QString::fromLocal8Bit("粗调前"));
        ui->pushButton_BACK_2->setText(QString::fromLocal8Bit("粗调后"));
        ui->pushButton_LEFT_2->setText(QString::fromLocal8Bit("粗调左"));
        ui->pushButton_RIGHT_2->setText(QString::fromLocal8Bit("粗调右"));
        ui->pushButton_ChangeAdjust->setText(QString::fromLocal8Bit("粗调->微调"));
        break;
    case ADJUST1:
        AdjustSel = ADJUST2;
        ui->pushButton_FWD_2->setText(QString::fromLocal8Bit("微调前"));
        ui->pushButton_BACK_2->setText(QString::fromLocal8Bit("微调后"));
        ui->pushButton_LEFT_2->setText(QString::fromLocal8Bit("微调左"));
        ui->pushButton_RIGHT_2->setText(QString::fromLocal8Bit("微调右"));
        ui->pushButton_ChangeAdjust->setText(QString::fromLocal8Bit("微调->移动"));
        break;
    case ADJUST2:
        AdjustSel = MOVE_;
        ui->pushButton_FWD_2->setText(QString::fromLocal8Bit("前"));
        ui->pushButton_BACK_2->setText(QString::fromLocal8Bit("后"));
        ui->pushButton_LEFT_2->setText(QString::fromLocal8Bit("左"));
        ui->pushButton_RIGHT_2->setText(QString::fromLocal8Bit("右"));
        ui->pushButton_ChangeAdjust->setText(QString::fromLocal8Bit("移动->粗调"));
        break;
    default:
        break;
    }
}


void SettingDialog::on_pushButton_FWD_clicked()
{
    printf("move forward\n");
    int GetDistance = ui->lineEdit_MoveDistance->text().toInt();
    Mediator::GetIns()->MachineOp(RELATIVE_MOVE_UP,GetDistance,1);
}


void SettingDialog::on_pushButton_BACK_clicked()
{
    int GetDistance = ui->lineEdit_MoveDistance->text().toInt();
    Mediator::GetIns()->MachineOp(RELATIVE_MOVE_DOWN,GetDistance,1);
}

void SettingDialog::on_pushButton_LEFT_clicked()
{
    int GetDistance = ui->lineEdit_MoveDistance->text().toInt();
    Mediator::GetIns()->MachineOp(RELATIVE_MOVE_LEFT,GetDistance,1);
}

void SettingDialog::on_pushButton_RIGHT_clicked()
{
   int GetDistance = ui->lineEdit_MoveDistance->text().toInt();
   Mediator::GetIns()->MachineOp(RELATIVE_MOVE_RIGHT,GetDistance,1);
}


bool SettingDialog::eventFilter(QObject* obj,QEvent *event)
{
    static bool isTouched = false;
    static int CurrentXPos = 0;//motion::GetIns()->motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS);
    static int CurrentYPos = 0;//motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Y_AXIS_MOTOR);
    switch (event->type()) {
    case QEvent::TouchBegin:
        isTouched = true;
        if(obj == ui->pushButton_Left_Limit)
        {
            if(NOT_ORIGIN == (Mediator::GetIns()->GetState() & NOT_ORIGIN))
            {
                ui->lineEdit_XLimit->setText(QString::fromLocal8Bit("请复位后设置"));
            }
            else
            {
                int LimitPos = -210000000;
                if(左方向为原点)
                    LimitPos = 210000000;
                motion::GetIns()->CurrentCard()->SetLimit(X_AXIS_MOTOR, LimitPos,左方向为原点);
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 3;
                Mediator::GetIns()->MachineOp(MOVE_LEFT);
                X_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_LeftLimit2)
        {
            if(NOT_ORIGIN == (Mediator::GetIns()->GetState() & NOT_ORIGIN))
            {
                ui->lineEdit_XLimit->setText(QString::fromLocal8Bit("请复位后设置"));
            }
            else
            {
                int LimitPos = -210000000;
                if(左方向为原点)
                    LimitPos = 210000000;
                motion::GetIns()->CurrentCard()->SetLimit(X_AXIS_MOTOR, LimitPos,左方向为原点);
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 3;
                Mediator::GetIns()->MachineOp(MOVE_RIGHT);
                X_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_FWD_2)
        {
            CurrentYPos = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Y_AXIS_MOTOR);
            if(MOVE_  == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_UP);
                Y_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST1 == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_UP);
                Y_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST2 == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 125;
                Mediator::GetIns()->MachineOp(MOVE_UP);
                Y_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_BACK_2)
        {
            CurrentYPos = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Y_AXIS_MOTOR);
            if(MOVE_  == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_DOWN);
                Y_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST1 == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_DOWN);
                Y_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST2 == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 125;
                Mediator::GetIns()->MachineOp(MOVE_DOWN);
                Y_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_LEFT_2)
        {
            CurrentXPos = motion::GetIns()->motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS);
            if(MOVE_  == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_LEFT);
                X_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST1 == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_LEFT);
                X_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST2 == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 125;
                Mediator::GetIns()->MachineOp(MOVE_LEFT);
                X_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_RIGHT_2)
        {
            CurrentXPos = motion::GetIns()->motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS);
            if(MOVE_  == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_RIGHT);
                X_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST1 == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_RIGHT);
                X_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST2 == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 125;
                Mediator::GetIns()->MachineOp(MOVE_RIGHT);
                X_AXIS_SPEED = origin_x_spd;
            }
        }
        break;
    case QEvent::MouseButtonPress:
        if(isTouched) break;
        if(obj == ui->pushButton_Left_Limit)
        {
            if(NOT_ORIGIN == (Mediator::GetIns()->GetState() & NOT_ORIGIN))
            {
                ui->lineEdit_XLimit->setText(QString::fromLocal8Bit("请复位后设置"));
            }
            else
            {

                int LimitPos = -210000000;
                if(左方向为原点)
                    LimitPos = 210000000;
                motion::GetIns()->CurrentCard()->SetLimit(X_AXIS_MOTOR, LimitPos,左方向为原点);


                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_LEFT);
                X_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_LeftLimit2)
        {
            if(NOT_ORIGIN == (Mediator::GetIns()->GetState() & NOT_ORIGIN))
            {
                ui->lineEdit_XLimit->setText(QString::fromLocal8Bit("请复位后设置"));
            }
            else
            {
                int LimitPos = -210000000;
                if(左方向为原点)
                    LimitPos = 210000000;
                motion::GetIns()->CurrentCard()->SetLimit(X_AXIS_MOTOR, LimitPos,左方向为原点);
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_RIGHT);
                X_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_FWD_2)
        {
            CurrentYPos = motion::GetIns()->motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Y_AXIS_MOTOR);
            if(MOVE_  == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_UP);
                Y_AXIS_SPEED = origin_x_spd;

            }
            else if(ADJUST1 == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_UP);
                Y_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST2 == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 125;
                Mediator::GetIns()->MachineOp(MOVE_UP);
                Y_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_BACK_2)
        {
            CurrentYPos = motion::GetIns()->motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Y_AXIS_MOTOR);
            if(MOVE_  == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_DOWN);
                Y_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST1 == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_DOWN);
                Y_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST2 == AdjustSel)
            {
                int origin_x_spd = Y_AXIS_SPEED;
                Y_AXIS_SPEED = Y_AXIS_SPEED / 125;
                Mediator::GetIns()->MachineOp(MOVE_DOWN);
                Y_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_LEFT_2)
        {
            CurrentXPos = motion::GetIns()->motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS);
            if(MOVE_  == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_LEFT);
                X_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST1 == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_LEFT);
                X_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST2 == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 125;
                Mediator::GetIns()->MachineOp(MOVE_LEFT);
                X_AXIS_SPEED = origin_x_spd;
            }
        }
        else if(obj == ui->pushButton_RIGHT_2)
        {
            CurrentXPos = motion::GetIns()->motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS);
            if(MOVE_  == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_RIGHT);
                X_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST1 == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 2;
                Mediator::GetIns()->MachineOp(MOVE_RIGHT);
                X_AXIS_SPEED = origin_x_spd;
            }
            else if(ADJUST2 == AdjustSel)
            {
                int origin_x_spd = X_AXIS_SPEED;
                X_AXIS_SPEED = X_AXIS_SPEED / 125;
                Mediator::GetIns()->MachineOp(MOVE_RIGHT);
                X_AXIS_SPEED = origin_x_spd;
            }
        }
        break;
    case QEvent::TouchEnd:
    case QEvent::MouseButtonRelease:
        if(obj == ui->pushButton_Left_Limit || obj == ui->pushButton_LeftLimit2)
        {
            Mediator::GetIns()->MachineOp(MOVE_STOP);
            if(NOT_ORIGIN == (Mediator::GetIns()->GetState() & NOT_ORIGIN))
            {
                ui->lineEdit_XLimit->setText(QString::fromLocal8Bit("请复位后设置"));
            }
            else
            {
                clock_t clk = clock();
                while(false == Mediator::GetIns()->isAxisStop(X_AXIS_MOTOR))
                {
                    if((clock()-clk) > 1200)
                        break;
                }
                X_AXIS_LIMIT = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS);
                motion::GetIns()->CurrentCard()->SetLimit(X_AXIS_MOTOR, X_AXIS_LIMIT,左方向为原点);


                ui->lineEdit_XLimit->setText(QString::number(X_AXIS_LIMIT));
                Preference::GetIns()->prj->WriteSettings();
            }
        }
        else if( obj == ui->pushButton_LEFT_2 || obj == ui->pushButton_RIGHT_2)

        {
            Mediator::GetIns()->MachineOp(MOVE_STOP);
            if(ADJUST2 == AdjustSel || ADJUST1 == AdjustSel)
            {
                clock_t clk = clock();
                while(false == Mediator::GetIns()->isAxisStop(X_AXIS_MOTOR))
                {
                    if((clock()-clk) > 1200)
                        break;
                }
                int NCurrentXPos = motion::GetIns()->motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,X_AXIS_MOTOR) - CurrentXPos;
                int FinalPos = ui->lineEdit_XAxisCamDis->text().toInt() + NCurrentXPos;
                ui->lineEdit_XAxisCamDis->setText(QString::number(FinalPos));
                on_pushButton_SavePara_clicked();
            }
        }

        else if(obj == ui->pushButton_FWD_2 || obj == ui->pushButton_BACK_2 )
        {
            Mediator::GetIns()->MachineOp(MOVE_STOP);
            if(ADJUST2 == AdjustSel || ADJUST1 == AdjustSel)
            {
                clock_t clk = clock();
                while(false == Mediator::GetIns()->isAxisStop(Y_AXIS_MOTOR))
                {
                    if((clock()-clk) > 1200)
                        break;
                }
                int NCurrentYPos = motion::GetIns()->CurrentCard()->ReadInputBit(YANWEI_AXIS_CMD_POS,Y_AXIS_MOTOR) - CurrentYPos;
                int FinalPos = ui->lineEdit_YAxisCamDis->text().toInt() + NCurrentYPos;
                ui->lineEdit_YAxisCamDis->setText(QString::number(FinalPos));
                on_pushButton_SavePara_clicked();
            }
        }
        isTouched = false;
        break;
    default:
        break;
    }

    return QDialog::eventFilter(obj,event);
}



void SettingDialog::on_pushButton_SavePara_2_clicked()
{
    //两张图片比较是否掉线
    cv::Mat Image_Try   = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
    cv::Mat Snap_Image       = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
    if(true == MSerialsCamera::isEqual(Snap_Image,Image_Try))
    {
        for (int i = 0;;i++)
        {
            Sleep(100);
            if(MSerialsCamera::init_camera() > 0) {
                    Snap_Image       = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
                    break;
            }
            else if (i>3)
            {
                QString fileName = QFileDialog::getOpenFileName(NULL,
                    tr("Open Image"), "",
                    tr("All Files (*)"));
                cv::Mat Image = cv::imread(fileName.toLocal8Bit().data());
                if(Image.empty())   return;
                Snap_Image = Image.clone();
                CvCvtColor(Image,Snap_Image,CV_BGR2GRAY);
                cv::resize(Snap_Image, Image, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT), 0.0,0.0, cv::INTER_CUBIC);
                MSerialsCamera::VoidImage() = Image.clone();
                break;
            }
        }
    }
    try{
    HalconCpp::HObject Hobj,ROI;
    GenRectangle1(&ROI,CHECK_R1,CHECK_C1,CHECK_R2,CHECK_C2);
    Excv::MatToHObj(Snap_Image,Hobj);
    Excv::h_disp_obj(Hobj,Mediator::GetIns()->SetDispHd);
    SetDraw(Mediator::GetIns()->SetDispHd,"margin");
    SetColor(Mediator::GetIns()->SetDispHd,"yellow");
    DispObj(ROI,Mediator::GetIns()->SetDispHd);
    }catch(cv::Exception ex)
    {
        HalconCpp::SetTposition(Mediator::GetIns()->SetDispHd,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->SetDispHd,"red");
        HalconCpp::WriteString(Mediator::GetIns()->SetDispHd,ex.what());
    }
}

void SettingDialog::on_pushButton_AngleUp_clicked()
{

    try
    {
        CAMERA_ANGLE = CAMERA_ANGLE + 0.025;
        HalconCpp::HObject Hobj;
        cv::Mat Snap_Image  = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
        Excv::MatToHObj(Snap_Image,Hobj);
        Excv::h_disp_obj(Hobj,Mediator::GetIns()->SetDispHd);
        ui->lineEdit_CamAngle->setText(QString::number(CAMERA_ANGLE,'g',8));
        on_pushButton_SavePara_clicked();
    }catch(cv::Exception ex)
    {
        HalconCpp::SetTposition(Mediator::GetIns()->SetDispHd,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->SetDispHd,"red");
        HalconCpp::WriteString(Mediator::GetIns()->SetDispHd,ex.what());
    }
}

void SettingDialog::on_pushButton_Angle_Down_clicked()
{
    try
    {
        CAMERA_ANGLE = CAMERA_ANGLE - 0.025;
        HalconCpp::HObject Hobj;
        cv::Mat Snap_Image  = MSerialsCamera::GetMachineImage(MSerialsCamera::IMAGE_FLIPED,CAMERA_ANGLE).clone();
        Excv::MatToHObj(Snap_Image,Hobj);
        Excv::h_disp_obj(Hobj,Mediator::GetIns()->SetDispHd);
        ui->lineEdit_CamAngle->setText(QString::number(CAMERA_ANGLE,'g',8));
        on_pushButton_SavePara_clicked();
    }catch(cv::Exception ex)
    {
        HalconCpp::SetTposition(Mediator::GetIns()->SetDispHd,10,0);
        HalconCpp::SetColor(Mediator::GetIns()->SetDispHd,"red");
        HalconCpp::WriteString(Mediator::GetIns()->SetDispHd,ex.what());
    }

}
