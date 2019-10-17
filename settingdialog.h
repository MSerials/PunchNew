#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    enum{MOVE_,ADJUST1,ADJUST2};
    int AdjustSel = MOVE_;
    Q_OBJECT

public:
    void Close_();
    bool isOpened = false;
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

protected:
    void MachineOp(int Sel, int SecondSel = 0);
    bool eventFilter(QObject* obj,QEvent *event);
private slots:
    void on_pushButton_KeyBoard_clicked();

    void on_pushButton_CloseSet_clicked();

    void on_pushButton_SavePara_clicked();

    void on_pushButton_CabliSize_clicked();

    void on_pushButton_Cabli_clicked();

    void on_pushButton_SaveAsCalbiImage_clicked();



    void on_pushButton_ChangeAdjust_clicked();

    void on_pushButton_Cabli_File_clicked();

    void on_pushButton_FWD_clicked();

    void on_pushButton_BACK_clicked();

    void on_pushButton_LEFT_clicked();

    void on_pushButton_RIGHT_clicked();

    void on_pushButton_SavePara_2_clicked();

    void on_pushButton_AngleUp_clicked();

    void on_pushButton_Angle_Down_clicked();

private:
    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
