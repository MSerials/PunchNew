#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTouchEvent>
#include "HalconCpp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum{
        NOP,LIMIT_UP,LIMIT_DOWN
    };

public:
    void ShowCounter(QString str);
    explicit MainWindow(QWidget *parent = 0);

    int Operation = NOP;

    void OnTimer();

    int GetCounter();

    void Init(const char * program_name = "");

    static unsigned int CheckVersion(void*);

    ~MainWindow();
    static MainWindow* ThisWindow;
private slots:
    void on_pushButton_Quit_clicked();

    void on_pushButton_ParamSet_clicked();

    void MachineOp(int Sel, int SecondSel = 0);

    bool eventFilter(QObject *obj, QEvent *event);

    static void show_State_Ex();
    void show_State();

    void on_pushButton_Run_clicked();

    void on_pushButton_Stop_2_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_ModelSet_clicked();

    static void   UpdateHistoryInfo(std::string Info);

     void  UpdateHistory(std::string Info);

     void on_pushButton_clicked();

     void on_pushButton_Punch_clicked();

     void on_pushButton_Stop_clicked();

     void on_pushButton_Press_clicked();

     void on_pushButton_Seperate_clicked();

     void on_pushButton_Min_clicked();

     //void on_pushButton_NormalSet_clicked();

     void on_pushButton_NormalSet_clicked();

private:
    std::string ProgmaName;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
