#include "mainwindow.h"
#include <QApplication>
#include "global.h"
#include <iostream>
#include <cryptdialog.h>
#include "windows.h"

//如果安装了wdk请增加这下面的东西，并且注意版本
#pragma comment(lib,"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.17763.0/ucrt/x64/ucrt.lib")
//所有的lib链接库在这里,不知道什么bug，不能使用相对路径
#pragma comment(lib,"D:/ProjectFile/third_party/opencv4.0.1/x64/vc15/lib/opencv_world401.lib")
#pragma comment(lib,"D:/ProjectFile/third_party/crypt/vc15/cryptlib.lib")
#pragma comment(lib,"D:/ProjectFile/third_party/yanwei/inc/IMCnet.lib")
#pragma comment(lib,"D:/ProjectFile/third_party\\halcon12\\halconcpp.lib")
#pragma comment(lib,"D:/ProjectFile/ChongChuang12\\x64\\Release\\MSerialsCV.lib")
#pragma comment(lib,"D:/ProjectFile/ChongChuang12\\x64\\Release\\MSerialsCamera.lib")



void Init()
{
    QDir q;
    q.mkdir("C:/IMAGE");
    q.mkdir(CABLI);
    q.mkdir(MODEL);
    std::string Str = _global::CameraResolution()[0];
    std::cout << Str <<std::endl;
    Str = _global::LineMethod()[0];
    _global::GetIns()->Image   = cv::Mat(IMAGE_HEIGHT,IMAGE_WIDTH,CV_8UC1,cv::Scalar(220,220,220));
}

int main(int argc, char *argv[])
{
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    QApplication a(argc, argv);
    CryptDialog CryptDlg;
    if(!CryptDlg.Init())    CryptDlg.exec();
    Init();
    MainWindow w;
    w.Init(std::string(argv[0]).c_str());
    w.show();
    return a.exec();
}
