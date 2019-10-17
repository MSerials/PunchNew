#include "global.h"
#include <QDateTime>
#include "cryptdialog.h"
#include "ui_cryptdialog.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "../../third_party/crypt/include/md5.h"
using namespace CryptoPP;
#define REGEDITPATH	L"Software\\MicroSoftReg"
#define ORIGINDATE	L"2019-1-1"

bool isOK = false;

CryptDialog::CryptDialog(char* Crypt,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CryptDialog)
{
    ui->setupUi(this);
   // this->setModal(true);
    m_Crypt  = &Crypt;
    this->setWindowFlags(Qt::FramelessWindowHint);

}

CryptDialog::~CryptDialog()
{
    delete ui;
}

bool CryptDialog::Init(){

        std::string Board = _global::GetBoardInfo().toStdString();
        std::string FirstCyrpted,SecondCyrpted;
        size_t StrLen = strlen(Board.c_str());
        if (StrLen < 2)
        {
            QMessageBox::information(NULL,"Tip",QString::fromLocal8Bit("验证信息无效，可能密钥不对或者没用管理员运行"));
            exit(0);
        }
        CryptoPP::byte FirstCyrptAsBoard[36] = { 0 };	//因为要转成字符串，所以+2个作为结束符号的替补
        //第一次算密时候给出serials
        Weak::MD5 md5;
        md5.Update((CryptoPP::byte*)Board.c_str(), StrLen);//strlen=11
        md5.Final(FirstCyrptAsBoard);
        for (int i = 0; i < 16; i++)
        {
            char Tmp[6];
            sprintf_s(Tmp, "%02X\0\0", FirstCyrptAsBoard[i]);
            std::string TmpEd(Tmp);
            FirstCyrpted += TmpEd;
        }
//加密过一次从打印出来
        ui->lineEditSerials->setText(QString(FirstCyrpted.c_str()));

        md5.Update((CryptoPP::byte*)FirstCyrpted.c_str(), strlen(FirstCyrpted.c_str()));//strlen=11
        md5.Final(FirstCyrptAsBoard);
        for (int i = 0; i < 16; i++)
        {
            char Tmp[6];
            sprintf_s(Tmp, "%02X\0\0", FirstCyrptAsBoard[i]);
            std::string TmpEd(Tmp);
            SecondCyrpted += TmpEd;
        }

    #ifdef NO_MOTION
        printf_s("机器号码:%s  字节长度%zd  加密后 %s  加密后2 %s\n",Board.c_str(),StrLen,FirstCyrpted.c_str(), SecondCyrpted.c_str());
    #endif
        long long stamp;
        QString Serials;
        int days ,Res = _global::GetDate(stamp, Serials, days);

        if (ERROR_OK_ != Res) {
            QMessageBox::information(NULL,"Tip",QString::fromLocal8Bit("可能没有管理员权限运行或者没有注册过！"));
        }
        else if (0 == strcmp(Serials.toStdString().data(),SecondCyrpted.c_str()))
        {
            int tYear = QDateTime::currentDateTime().toString("yyyy").toInt();
            int tMonth = QDateTime::currentDateTime().toString("MM").toInt();
            int tDay = QDateTime::currentDateTime().toString("dd").toInt();
            printf("year %d, month %d, day %d\n",tYear,tMonth,tDay);
            int Today = _global::GetThisYearDay(tYear, tMonth, tDay);
            int RecordDay = stamp;
            if ((Today - RecordDay) < 0)
            {
                QMessageBox::information(NULL,"Tip",QString::fromLocal8Bit("数据异常！"));
            }
            else if ((Today - RecordDay) < days)
            {
                printf("验证正确\n");
                close();
                return true;
            }
            else {
                QMessageBox::information(NULL,"Tip",QString::fromLocal8Bit("试用期已过！"));
            }
        }
        else
        {
    #ifdef TEST
            std::cout << "cal " << SecondCyrpted.c_str() << " in reg is " << Serials.toLocal8Bit().data() << std::endl;

    #endif
            QMessageBox::information(NULL,"Tip",QString::fromLocal8Bit("验证码不对！"));

        }
        return false;
}



void CryptDialog::on_pushButton_2_clicked()
{
    exit(0);
}

void CryptDialog::on_pushButton_clicked()
{
    QString Str = ui->lineEdit_Crypt->text();
    _global::RegSetValueW(Str);
    Init();
}
