#include "debuginfo.h"
#include "MSerialsCV.h"
#include "cameras.h"
#include "preferences.h"
#include <windows.h>
#include <mutex>
#include "MSerialsCore/motion.h"
#include "vector"
#include "list"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <omp.h>
#include <QDir>
#include <unordered_map>
#include <exception>
#include "common.h"



#define ERRORLOG(ErrorInfo)    \
{   \
    Mediator::GetIns()->UpdateMessage(ErrorInfo);\
}

#ifndef GLOBAL_H
#define GLOBAL_H

#define CABLI "C:/IMAGE/Cabli"
#define MODEL "C:/IMAGE/Model"
#define INI   "prj"


#if 1
#define IMAGE_WIDTH     int(Preference::GetIns()->prj->CamWidth[MODE_INDEX])
#define IMAGE_HEIGHT    int(Preference::GetIns()->prj->CamHeight[MODE_INDEX])
#else
#define MODE_INDEX      1
#define IMAGE_WIDTH     1280
#define IMAGE_HEIGHT    1024
#endif

#define ERROR_ADMIN_	-1
#define ERROR_OK_		0
#define ERROR_AUTHOR_	3
#define ERROR_TIMEOUT_	4


//定义滤波时间间隔，因为冲床感应器不稳定，用时间去滤波
#define     TIME_GAP    100
//定义机器的状态
#define     NoError         (0)
#define     NOT_ORIGIN      (1)
#define     XAXISALM        (1<<1)
#define     YAXISALM        (1<<2)
#define     RUN             (1<<3)
#define     STOP            (1<<4)
#define     PAUSE           (1<<5)
#define     NO_CAMERA       (1<<6)
#define     NO_MOTION_CARD  (1<<7)
#define     ERROR_PUNCHING  (1<<8)
#define     ORIGING         (1<<9)

//操作按钮定义
#define     MOVE_UP     (1)
#define     MOVE_LEFT   (2)
#define     MOVE_RIGHT  (3)
#define     MOVE_DOWN   (4)
#define     MOVE_STOP   (5)
#define     RELATIVE_MOVE_UP    (6)
#define     RELATIVE_MOVE_DOWN  (7)
#define     RELATIVE_MOVE_LEFT  (8)
#define     RELATIVE_MOVE_RIGHT (9)
#define     MACHINE_START   (10)
#define     MACHINE_PAUSE   (11)
#define     MACHINE_STOP    (12)
#define     MACHINE_ORIGIN  (13)
#define     MACHINE_PUNCH   (14)
#define     MACHINE_SEPRA   (15)





class MEvent
{
private:
    std::mutex mtx;
    HANDLE evt;
    int state;
public:
    MEvent():
        state(0)
    {
        evt = CreateEventW(FALSE,TRUE,FALSE,NULL);
        ::ResetEvent(evt);
    }
    const HANDLE &get(){return evt;}
    int SetEvent() {std::lock_guard<std::mutex> lck(mtx);::SetEvent(evt);return (state = 1);}
    int ResetEvent(){std::lock_guard<std::mutex> lck(mtx);::ResetEvent(evt);return (state = 0);}
    int &State() { return state; }
};

class _global{

    _global(){
        /*
        LineMethod()[LINES_HORIZONTAL_AI] = "智能横向排列";
        LineMethod()[LINES_HORIZONTAL] = "圆形横向排列";
        LineMethod()[LINES_VERTICAL_AI] = "智能纵向排列";
        LineMethod()[LINES_VERTICAL] = "纵向排列";
        LineMethod()[LINES_HORIZONTAL_FILL] = "横向排列扩增";
        LineMethod()[LINES_HORIZONTAL_USER] = "横排左对齐";
        LineMethod()[DOUBLE_VERTICAL] = "上下双面";
        printf("global\n");
        */

    }

public:
static _global    *GetIns(){
    static _global _g;
    return &_g;
}

    static long GetThisYearDay(int year, int month, int day)
    {
        int counterOfOddYear = (year / 4);
        if (!(year % 4)) {
            counterOfOddYear--;
            long allday = (year - 1) * 365 + counterOfOddYear;
            switch (month)
            {
            case 1: return (day + allday);
            case 2: return (day + allday + 31);
            case 3: return (day + allday + 60);
            case 4: return (day + allday + 91);
            case 5: return (day + allday + 121);
            case 6: return (day + allday + 152);
            case 7: return (day + allday + 182);
            case 8: return (day + allday + 213);
            case 9: return (day + allday + 244);
            case 10: return (day + allday + 274);
            case 11: return (day + allday + 305);
            case 12: return (day + allday + 335);
            default:return 0;
            }
        }
        else
        {
            long allday = (year) * 365 + counterOfOddYear;
            switch (month)
            {
            case 1: return (day + allday);
            case 2: return (day + allday + 31);
            case 3: return (day + allday + 59);
            case 4: return (day + allday + 90);
            case 5: return (day + allday + 120);
            case 6: return (day + allday + 151);
            case 7: return (day + allday + 181);
            case 8: return (day + allday + 212);
            case 9: return (day + allday + 243);
            case 10: return (day + allday + 273);
            case 11: return (day + allday + 304);
            case 12: return (day + allday + 334);
            default:return 0;
            }

        }
    }



    cv::Mat Image;//
    cv::Mat map1, map2;        bool ToRemap = false;

    static std::unordered_map<int,std::string> &LineMethod(){
        //采用双锁检测才保证多线程无bug 不过这里无所谓
        static std::unordered_map<int,std::string> LM;
        static bool isOK = false;
        if(!isOK)
        {
            isOK = true;
            LineMethod()[LINES_HORIZONTAL] = "横向排列";
            LineMethod()[LINES_VERTICAL] = "纵向排列";
            LineMethod()[LINES_HORIZONTAL_FILL] = "横向扩图";
            LineMethod()[LINES_HORIZONTAL_USER] = "横向扩图调参";




            /*
            LineMethod()[LINES_HORIZONTAL_AI] = "智能横向排列";
            LineMethod()[LINES_HORIZONTAL] = "圆形横向排列";
            LineMethod()[LINES_VERTICAL_AI] = "智能纵向排列";
            LineMethod()[LINES_VERTICAL] = "纵向排列";
            LineMethod()[LINES_HORIZONTAL_FILL] = "横向排列扩增";
            LineMethod()[LINES_HORIZONTAL_USER] = "横排左对齐";
            LineMethod()[DOUBLE_VERTICAL] = "上下双面";
            */
        }
        return LM;
    }

    //0 2048x1516
    //1 1600x1200
    //2 1280x1024
    //3 1024x768

    static std::unordered_map<int,std::string> &DoubleModel(){
        //采用双锁检测才保证多线程无bug 不过这里无所谓
        static std::unordered_map<int,std::string> LM;
        static bool isOK = false;
        if(!isOK)
        {
            isOK = true;
            LM[0] = "单模具";
            LM[1] = "双模具";
            LM[2] = "三模具";
        }
        return LM;
    }

    static std::unordered_map<int,std::string> &CameraResolution(){
        //采用双锁检测才保证多线程无bug 不过这里无所谓
        static std::unordered_map<int,std::string> LM;
        static bool isOK = false;
        if(!isOK)
        {            
            isOK = true;
            LM[0] = "像素:1600x1184";
            LM[1] = "像素:1280x960";
            LM[2] = "像素:1024x819";
            LM[3] = "像素:800x600";
            LM[4] = "像素:640x480";
            LM[5] = "像素:320x240";
        }
        return LM;
    }





    static QString GetBoardInfo()
    {
    QString cpu_id = "";
    QProcess p(0);
    //p.start("wmic BaseBoard get SerialNumber");
    p.start("wmic CPU get ProcessorID");
    p.waitForStarted();
    p.waitForFinished();
    cpu_id = QString::fromLocal8Bit(p.readAllStandardOutput());
    //cpu_id = cpu_id.remove("SerialNumber").trimmed();
    cpu_id = cpu_id.remove("ProcessorID").trimmed();
    return cpu_id;
    }

    static void RegSetValue(QString Value,QString Key = "HKEY_CURRENT_USER\\Software\\MicroSoftReg")
    {
        QSettings settings(Key,QSettings::NativeFormat);
        settings.setValue("T", Value);//写
    }

    static QString RegGetValue(QString Key = "HKEY_CURRENT_USER\\Software\\MicroSoftReg")
    {
        QSettings settings(Key,QSettings::NativeFormat);
        return settings.value("T").toString();
    }

    static void SChange(QString &s)
    {
        s.replace("A", "0");
        s.replace("B", "1");
        s.replace("C", "2");
        s.replace("D", "3");
        s.replace("E", "4");
        s.replace("F", "5");
        s.replace("G", "6");
        s.replace("H", "7");
        s.replace("I", "8");
        s.replace("J", "9");
    }


    static void SChangeInv(QString &s)
    {
        s.replace("0", "A");
        s.replace("1", "B");
        s.replace("2", "C");
        s.replace("3", "D");
        s.replace("4", "E");
        s.replace("5", "F");
        s.replace("6", "G");
        s.replace("7", "H");
        s.replace("8", "I");
        s.replace("9", "J");
    }


    static QStringList Split(QString Str,const char* reg = "[Z]+"){
        return Str.split(QRegExp(reg));
    }


    // Certify 对话框
    static int GetDate(long long &TimeStamp, QString &Serials, int &days)
    {
        TimeStamp = -1;
        QString RegVar = RegGetValueW();
        std::vector<QString> Se;
        if(RegVar.isEmpty())
            return ERROR_AUTHOR_;
        for(auto QStr:RegVar.split(QRegExp("[Z]+")))
        {
            Se.push_back(QStr);
        }
                try
                {
                    Serials = Se.at(0);


                    SChange(Se.at(1));
                    days = (Se.at(1)).toInt();

                     SChange(Se.at(2));
                     int year = (Se.at(2)).toInt();

                     SChange(Se.at(3));
                     int month = (Se.at(3)).toInt();

                     SChange(Se.at(4));
                     int day = (Se.at(4)).toInt();

                     bool isDateOK = true;
                     //year 2099
                     if (year < 2000 || year > 2099)
                     {
                         isDateOK = false;
                     }

                     if (month < 1 || month >12)
                     {
                         isDateOK = false;
                     }

                     if (day < 1 || day > 31)
                     {
                         isDateOK = false;
                     }

                     if (!isDateOK)
                     {
                         QMessageBox::information(NULL,"Error",QString::fromLocal8Bit("日期错误"));
                         return ERROR_AUTHOR_;
                     }

                     long GetDays = GetThisYearDay(year, month, day);
                     TimeStamp = GetDays;
                }
                catch (std::out_of_range e)
                {
                    printf_s("注册表分割失败\n");
                    return ERROR_AUTHOR_;
                }

                return ERROR_OK_;
    }

};


#endif // GLOBAL_H
