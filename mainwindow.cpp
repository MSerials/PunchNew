#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingdialog.h"
#include <iostream>
#include <QTouchEvent>
#include <QCoreApplication>
#include <QDialog>
#include <QFileDialog>
#include <list>
#include "modelsetdialog.h"
#include "global.h"
#include "opencv.hpp"
#include "mediator.h"
#include "excv.h"
#include <QMessageBox>
#include <QTextCodec>
#include "cryptdialog.h"
#include <string>
#include <iostream>
#include <fstream>
#include <QTimer>
#include "normalcontroldialog.h"
#include "checkprocess.h"

//不常用代码，但是必须存在的
void CheckMutex();
void MkDir();

MainWindow *pMainWin = nullptr;
int counter = 0;
int oldCounter = 0;
clock_t clk = clock();

void show_counter()
{
    QString str = QString::fromLocal8Bit("冲压次数:") + QString::number(counter);
    if(nullptr != pMainWin)
        pMainWin->ShowCounter(str);
    counter++;
    clock_t nclk = clock();
    if((nclk - clk) > 10000){
        clk = nclk;
        PUNCH_COUNTER = counter;
        Preference::GetIns()->prj->WriteSettings();
    }
    if((counter > PUNCH_LIMIT) && (0 < PUNCH_LIMIT))
        Mediator::GetIns()->MachineOp(MACHINE_STOP);
}

void MainWindow::ShowCounter(QString str)
{
    ui->label_Qty->setText(str);
}


void MainWindow::OnTimer()
{
    static int TickCounter = 0;
    switch(Operation)
    {
    case NOP:
        TickCounter = 0;
        break;
    case LIMIT_UP:
        if(0==TickCounter){
            PUNCH_LIMIT++;
            QString LimitCounter = QString::fromLocal8Bit("冲压限次：")+QString::number(PUNCH_LIMIT);
            ui->lineEdit_Limit->setText(LimitCounter);
        }
        else if(TickCounter>9){
            if(TickCounter > 3000) TickCounter = 3000;
            PUNCH_LIMIT += TickCounter;
            QString LimitCounter = QString::fromLocal8Bit("冲压限次：")+QString::number(PUNCH_LIMIT);
            ui->lineEdit_Limit->setText(LimitCounter);
        }
        TickCounter++;
        break;
    case LIMIT_DOWN:
        if(PUNCH_LIMIT <= 0){
        PUNCH_LIMIT = 0;
        QString LimitCounter = QString::fromLocal8Bit("冲压限次：")+QString::number(PUNCH_LIMIT);
        ui->lineEdit_Limit->setText(LimitCounter);
        break;
        }

        if(0==TickCounter){
            PUNCH_LIMIT--;
            QString LimitCounter = QString::fromLocal8Bit("冲压限次：")+QString::number(PUNCH_LIMIT);
            ui->lineEdit_Limit->setText(LimitCounter);
        }
        else if(TickCounter>9){
            if(TickCounter > 3000) TickCounter = 3000;
            PUNCH_LIMIT -= TickCounter;
            QString LimitCounter = QString::fromLocal8Bit("冲压限次：")+QString::number(PUNCH_LIMIT);
            ui->lineEdit_Limit->setText(LimitCounter);
        }
        TickCounter++;
        break;
    }
}

int MainWindow::GetCounter()
{
    return counter;
}



void MainWindow::Init(const char* pName)
{
#ifdef NO_MOTION
    cv::Mat Image;// = cv::imread(fileName.toLocal8Bit().data());
    Image = cv::imread("1.jpg");
    if(!Image.empty()) {
    cv::Mat  Snap_Image = Image.clone();
    CvCvtColor(Image,Snap_Image,CV_BGR2GRAY);
    cv::resize(Snap_Image, Image, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT), 0.0,0.0, cv::INTER_CUBIC);
    MSerialsCamera::VoidImage() = Image.clone();
    }
#endif

    CheckMutex();
    ProgmaName = std::string(pName);

    using namespace cv;
    FileStorage fs2("Cabli.yml", FileStorage::READ);
    fs2["cameraMatrix"] >> Mediator::GetIns()->m_cameraMatrix;
    fs2["distCoeffs"] >> Mediator::GetIns()->m_distCoeffs;
    std::cout<<"旋转" << Mediator::GetIns()->m_cameraMatrix<<std::endl;
    std::cout<<"畸变" << Mediator::GetIns()->m_distCoeffs<<std::endl;
    ERRORLOG("启动程序");
    MSerialsCamera::VoidImage();
    HalconCpp::HObject hobj;
    Excv::MatToHObj(MSerialsCamera::VoidImage(),hobj);

    //载入模具图片
    Mediator::GetIns()->SetDxfPara(DXFHL,DXFHU,DXFWL,DXFWU);
    if(PUNCHMODE == CAMERAMODE)
    {
        try{
            Mediator::GetIns()->Load_Dxf(Preference::GetIns()->prj->DxfPath.toLocal8Bit().toStdString().data(),Mediator::GetIns()->MainWindowDispHd,false);
        }catch(MException &e)
        {
            printf(e.what());
        }

        DispObj(hobj,Mediator::GetIns()->MainWindowDispHd);

        try{
            Mediator::GetIns()->Load_Model(Preference::GetIns()->prj->Model_Name.toLocal8Bit().toStdString().data(),Mediator::GetIns()->MainWindowDispHd,false);
        }catch(MException &e)
        {
            printf(e.what());
        }
    }
    else
    {
        try{
            Mediator::GetIns()->Load_Model(Preference::GetIns()->prj->Model_Name.toLocal8Bit().toStdString().data(),Mediator::GetIns()->MainWindowDispHd,false);
        }catch(MException &e)
        {
            printf(e.what());
        }

        DispObj(hobj,Mediator::GetIns()->MainWindowDispHd);

        try{
            Mediator::GetIns()->Load_Dxf(Preference::GetIns()->prj->DxfPath.toLocal8Bit().toStdString().data(),Mediator::GetIns()->MainWindowDispHd,false);
        }catch(MException &e)
        {
            printf(e.what());
        }
    }



    ui->Labell_Version->setText(QString::fromLocal8Bit("当前版本：") + QString(_VERSION));
    std::ofstream file("ver.dat");
    file << _VERSION;
    file.close();

    counter = PUNCH_COUNTER;
    show_counter();

    QString LimitCounter = QString::fromLocal8Bit("冲压限次：")+QString::number(PUNCH_LIMIT);
    ui->lineEdit_Limit->setText(LimitCounter);
    (HANDLE)_beginthreadex(NULL, 0,CheckVersion, this, 0, NULL);
}

unsigned int MainWindow::CheckVersion(void *pLvoid)
{

    CheckProcess CP;
    QList<quint64> ProcessID;
    CP.checkProcessRunning("Update.exe",ProcessID);
#ifdef NO_MOTION
    for(auto PID:ProcessID){
        std::cout <<"PID is " <<PID<<std::endl;
    }
#endif
    if(ProcessID.size()<1)
        QProcess::startDetached(tr("Update.exe"));
    return 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Quit_clicked()
{
    close();
}

void MainWindow::on_pushButton_ParamSet_clicked()
{
    //判断是否运动状态
    if(RUN == (Mediator::GetIns()->GetState()&RUN))
    {
        UpdateHistory("请停止后设置");
        return;
    }

    static SettingDialog * SetDlg = nullptr;
    if(nullptr != SetDlg)
    {
        delete SetDlg;
        SetDlg = nullptr;
    }

    SetDlg = new SettingDialog();

    SetDlg->show();
    SetDlg->exec();
}

void MainWindow::on_pushButton_ModelSet_clicked()
{
    //判断是否运动状态
    if(RUN == (Mediator::GetIns()->GetState()&RUN))
    {
        UpdateHistory("请停止后设置");
        return;
    }
    static ModelSetDialog *ModelSetDlg  = nullptr;
    if(nullptr != ModelSetDlg)
    {
        delete ModelSetDlg;
        ModelSetDlg = nullptr;
    }

    ModelSetDlg = new ModelSetDialog();
    ModelSetDlg->show();
    ModelSetDlg->exec();
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    static bool isTouched = false;
    switch (event->type()) {
    case QEvent::TouchBegin:
        isTouched = true;
        if(obj == ui->pushButton_Up)
        {
            Mediator::GetIns()->MachineOp(MOVE_UP);
        }
        else if(obj == ui->pushButton_Down)
        {
            Mediator::GetIns()->MachineOp(MOVE_DOWN);
        }
        else if(obj == ui->pushButton_Left)
        {
            Mediator::GetIns()->MachineOp(MOVE_LEFT);
        }
        else if(obj == ui->pushButton_Right)
        {
           Mediator::GetIns()->MachineOp(MOVE_RIGHT);
        }
        else if(obj == ui->pushButton_Miner)
        {
            Operation = LIMIT_DOWN;
        }
        else if(obj==ui->pushButton_Plus)
        {
            Operation = LIMIT_UP;
        }
        break;
    case QEvent::MouseButtonPress:
        if(isTouched) break;
        if(obj == ui->pushButton_Up)
        {
            Mediator::GetIns()->MachineOp(MOVE_UP);
        }
        else if(obj == ui->pushButton_Down)
        {
            Mediator::GetIns()->MachineOp(MOVE_DOWN);
        }
        else if(obj == ui->pushButton_Left)
        {
            Mediator::GetIns()->MachineOp(MOVE_LEFT);
        }
        else if(obj == ui->pushButton_Right)
        {
           Mediator::GetIns()->MachineOp(MOVE_RIGHT);
        }
        else if(obj == ui->pushButton_Miner)
        {
            Operation = LIMIT_DOWN;
        }
        else if(obj==ui->pushButton_Plus)
        {
            Operation = LIMIT_UP;
        }
        break;
    case QEvent::TouchEnd:
    case QEvent::MouseButtonRelease:
        if(obj == ui->pushButton_Up || obj == ui->pushButton_Down || obj == ui->pushButton_Left || obj == ui->pushButton_Right)
        {
            Mediator::GetIns()->MachineOp(MOVE_STOP);
        }
        else if(obj == ui->pushButton_Miner)
        {
            Operation = NOP;
            Preference::GetIns()->prj->WriteSettings();
        }
        else if(obj==ui->pushButton_Plus)
        {
            Operation = NOP;
            Preference::GetIns()->prj->WriteSettings();
        }
        isTouched = false;
        break;
    default:
        break;
    }
    return QMainWindow::eventFilter(obj,event);
}

void MainWindow::MachineOp(int Sel, int SecondSel)
{
    SecondSel = 1;
    printf("select %d\n",Sel);
}

void MainWindow::show_State_Ex()
{
    pMainWin->show_State();
}

void MainWindow::show_State()
{
        long long state = Mediator::GetIns()->GetState();
        if(PAUSE == (state&PAUSE))
        {
            ui->pushButton_Run->setText(QString::fromLocal8Bit("运行"));
        }
        else if(STOP == (state&STOP))
        {
            ui->pushButton_Run->setText(QString::fromLocal8Bit("运行"));
        }
        else if(RUN == (state&RUN))
        {
            ui->pushButton_Run->setText(QString::fromLocal8Bit("暂停"));
        }
}

void MainWindow::on_pushButton_Run_clicked()
{


#ifdef NO_MOTION
    static int counter = 5;
    counter--;
#endif
    if(RUN == (Mediator::GetIns()->GetState()&RUN))
    {
        Mediator::GetIns()->MachineOp(MACHINE_PAUSE);
    }
    else{
        Mediator::GetIns()->MachineOp(MACHINE_START);
    }
    show_State();
#ifdef NO_MOTION
if(counter>0)
{
    Mediator::GetIns()->MachineOp(MACHINE_STOP);
}
#endif
}

void MainWindow::on_pushButton_Stop_2_clicked()
{
    Mediator::GetIns()->MachineOp(MACHINE_STOP);
    ui->pushButton_Run->setText(QString::fromLocal8Bit("运行"));
    return;
    if(RUN == (Mediator::GetIns()->GetState()&RUN))
    {
        Mediator::GetIns()->MachineOp(MACHINE_PAUSE);
        ui->pushButton_Run->setText(QString::fromLocal8Bit("暂停"));
    }
    else{
        Mediator::GetIns()->MachineOp(MACHINE_STOP);
        ui->pushButton_Run->setText(QString::fromLocal8Bit("运行"));
    }
}

void MainWindow::on_pushButton_clear_clicked()
{
    counter = 0;
    show_counter();
}

void  MainWindow::UpdateHistoryInfo(std::string Info)
{
   pMainWin->UpdateHistory(Info);
}

void  MainWindow::UpdateHistory(std::string Info)
{
    static std::list<std::string> InfoMations;
    InfoMations.push_back(Info);
    while(InfoMations.size()>10) InfoMations.pop_front();
    std::string Msg;
    for(auto I:InfoMations){Msg += I+"\n";}
    ui->LabelHis->setText(QString::fromLocal8Bit(Msg.c_str()));
}

void MainWindow::on_pushButton_clicked()
{
    Mediator::GetIns()->MachineOp(MACHINE_ORIGIN);
}

void MainWindow::on_pushButton_Punch_clicked()
{
    Mediator::GetIns()->MachineOp(MACHINE_PUNCH);
}

void MainWindow::on_pushButton_Stop_clicked()
{
   // this->setCursor(QCursor(QPixmap("d:/Users/Lux/Desktop/ProjectFile/ChongChuang/res/Chongchuang.png")));
    /*
    static cv::Mat image(30000,30000,CV_8UC3,cv::Scalar(255,234,0));
    uchar *idata = image.data;
    for(int h = 0;h<image.rows;h++)
    {
        for(int w = 0;w<image.cols;w++)
        {
            idata[w*3] = rand()%255;
        }
        idata += image.cols*3;
    }

     cv::imwrite("a.jpg",image);
    HalconCpp::HObject hobj;
    Excv::MatToHObj(image,hobj);
    Excv::h_disp_obj(hobj,Mediator::GetIns()->MainWindowDispHd);




*/
    printf("ProcessState is %d  PunchState is %d GetPointState is %d\n",Mediator::GetIns()->evt_Process.State(),Mediator::GetIns()->evt_Punch.State(),Mediator::GetIns()->evt_GetPoint.State());
}

void MainWindow::on_pushButton_Press_clicked()
{
    Mediator::GetIns()->MachineOp(MACHINE_PUNCH);
    Sleep(200);
    Mediator::GetIns()->MachineOp(MACHINE_SEPRA);
}

void MainWindow::on_pushButton_Seperate_clicked()
{
    Mediator::GetIns()->MachineOp(MACHINE_SEPRA);
}

void MainWindow::on_pushButton_Min_clicked()
{
    showMinimized();
}

/*
void MainWindow::on_pushButton_NormalSet_clicked()
{
    //判断是否运动状态
    if(STOP == (Mediator::GetIns()->GetState()&STOP))
    {
        UpdateHistory("请停止后设置");
        return;
    }
    static NormalControlDialog *ModelSetDlg  = nullptr;
    if(nullptr != ModelSetDlg)
    {
        delete ModelSetDlg;
        ModelSetDlg = nullptr;
    }

    ModelSetDlg = new NormalControlDialog();
    ModelSetDlg->show();
    ModelSetDlg->exec();
}
*/

void MainWindow::on_pushButton_NormalSet_clicked()
{

    if(RUN == (Mediator::GetIns()->GetState()&RUN) ||
       PAUSE == (Mediator::GetIns()->GetState()&PAUSE)
       )
    {
        UpdateHistory("请停止后设置");
        return;
    }

    static NormalControlDialog *ModelSetDlg  = nullptr;
    if(nullptr != ModelSetDlg)
    {
        delete ModelSetDlg;
        ModelSetDlg = nullptr;
    }

    ModelSetDlg = new NormalControlDialog();
    ModelSetDlg->show();
    ModelSetDlg->exec();
     printf("show dialog");
}








































//不常用代码，但是必须
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  //  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); // 改为GBK编码
    ui->setupUi(this);
    pMainWin = this;


#ifdef SHOW_FULL_SCREEN
#ifndef DEBUGEX
    showFullScreen();
#endif
#endif
    setEnabled(true);

   // std::cout <<"ProgramName is "<<ProgmaName<<std::endl;
    Preference::GetIns()->prj->SetFilePos(QString(PRJ_PATH));

    setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_Up->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_Up->installEventFilter(this);
    ui->pushButton_Down->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_Down->installEventFilter(this);
    ui->pushButton_Left->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_Left->installEventFilter(this);
    ui->pushButton_Right->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_Right->installEventFilter(this);
    ui->pushButton_Miner->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_Miner->installEventFilter(this);
    ui->pushButton_Plus->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->pushButton_Plus->installEventFilter(this);
    int nWndWidth = ui->widget->width();
    int nWndHeight= ui->widget->height();
    HalconCpp::SetCheck("~father");
    HalconCpp::OpenWindow(0, 0, nWndWidth, nWndHeight, (Hlong)ui->widget->winId(), "visible", "", &    Mediator::GetIns()->MainWindowDispHd);
    HalconCpp::SetCheck("father");
    Mediator::GetIns()->UpdateHistory = UpdateHistoryInfo;
    Mediator::GetIns()->show_StartButton = show_State_Ex;
    Mediator::GetIns()->show_Counter = show_counter;
    //启动定时器
    QTimer *timer_io = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    timer_io->setInterval(23);
    connect(timer_io, &QTimer::timeout, [=]() {this->OnTimer();});
    timer_io->start();
}


void CheckMutex(){
    //防止程序多开
    HANDLE m_hMutex  =  ::CreateMutexW(NULL, FALSE,  L"PUNCH_____2019.1.5" );
    //  检查错误代码
    if  (GetLastError()  ==  ERROR_ALREADY_EXISTS)  {
     QMessageBox::warning(nullptr,QString::fromLocal8Bit("Error"),QString::fromLocal8Bit("请勿多开程序!如果频繁出现，请重启电脑"));
     CloseHandle(m_hMutex);
     m_hMutex  =  NULL;
     exit(0);
    }
}

void MkDir(){
    QDir dir;
    if(!dir.exists(CABLI)) dir.mkdir(CABLI);
    if(!dir.exists(MODEL)) dir.mkdir(MODEL);
    if(!dir.exists(CABLI)) dir.mkdir(INI);
}

std::string ToHex(long Data)
{
    char iHex[256] = {0};
    char Hex[9] = {0};
    sprintf(iHex,"%08X",Data);
    for(int i = 0;i < 4;i++)
    {
        Hex[4+i] = iHex[i];
        Hex[i] = iHex[4+i];
    }
    printf("%s \n",Hex);
    return std::string(Hex);
}
