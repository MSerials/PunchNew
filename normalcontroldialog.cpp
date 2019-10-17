#include "normalcontroldialog.h"
#include "ui_normalcontroldialog.h"
#include "mediator.h"

NormalControlDialog::NormalControlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NormalControlDialog)
{
    ui->setupUi(this);
#ifdef SHOW_FULL_SCREEN
    showFullScreen();
#endif
    ui->lineEdit_DXFHL->setText(QString::number(DXFHL,'g',6));
    ui->lineEdit_DXFHU->setText(QString::number(DXFHU,'g',6));
    ui->lineEdit_DXFWL->setText(QString::number(DXFWL,'g',6));
    ui->lineEdit_DXFWU->setText(QString::number(DXFWU,'g',6));
    switch(PUNCHMODE)
    {
    case CAMERAMODE:
        ui->pushButton_Switch->setText(QString::fromLocal8Bit("切换为数控模式"));
        break;
    case DIGITALMODE:
        ui->pushButton_Switch->setText(QString::fromLocal8Bit("切换为视觉模式"));
        break;
    default:
        PUNCHMODE = DIGITALMODE;
        ui->pushButton_Switch->setText(QString::fromLocal8Bit("切换为数控模式"));
        break;
    }

    int nWndWidth = ui->widget->width();
    int nWndHeight= ui->widget->height();
    HalconCpp::SetCheck("~father");
    HalconCpp::OpenWindow(0, 0, nWndWidth, nWndHeight, (Hlong)ui->widget->winId(), "visible", "", &    Mediator::GetIns()->NormalDisp);
    HalconCpp::SetCheck("father");
}

NormalControlDialog::~NormalControlDialog()
{
    delete ui;
}

void NormalControlDialog::on_pushButton_LoadDxf_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(NULL,tr("Open Dxf"), "Model",tr("All Files (*)"));
    try{
    Mediator::GetIns()->Load_Dxf(fileName.toLocal8Bit().data(),Mediator::GetIns()->NormalDisp,true);
    }catch(MException e)
    {
        QMessageBox::information(NULL,"Error", QString::fromLocal8Bit(e.what()).toLocal8Bit());
    }
}

void NormalControlDialog::on_pushButton_Save_clicked()
{
    DXFHL = ui->lineEdit_DXFHL->text().toDouble();
    DXFHU = ui->lineEdit_DXFHU->text().toDouble();
    DXFWL = ui->lineEdit_DXFWL->text().toDouble();
    DXFWU = ui->lineEdit_DXFWU->text().toDouble();
    PRJSAVE
}

void NormalControlDialog::on_pushButton_Switch_clicked()
{
    switch(PUNCHMODE)
    {
    case CAMERAMODE:
        PUNCHMODE = DIGITALMODE;
        ui->pushButton_Switch->setText(QString::fromLocal8Bit("切换为视觉模式"));
        break;
    case DIGITALMODE:
        ui->pushButton_Switch->setText(QString::fromLocal8Bit("切换为数控模式"));
        PUNCHMODE = CAMERAMODE;
        break;
    default:
        PUNCHMODE = DIGITALMODE;
        ui->pushButton_Switch->setText(QString::fromLocal8Bit("切换为视觉模式"));
        break;
    }
    PRJSAVE
}


void NormalControlDialog::on_pushButton_Close_clicked()
{
    close();
}



