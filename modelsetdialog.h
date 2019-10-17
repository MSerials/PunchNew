#ifndef MODELSETDIALOG_H
#define MODELSETDIALOG_H

#include <QDialog>
#include <QDebug>
#include "opencv.hpp"
#include "HalconCpp.h"


namespace Ui {
class ModelSetDialog;
}

class ModelSetDialog : public QDialog
{


    Q_OBJECT

public:
    enum{
        NOP,MIRROR_UPDOWN,MIRROR_LEFTRIGHT,ROTATE_CLOCK,ROTATE_ANTICLOCK,MODEL_MOVE_UP,MODEL_MOVE_DOWN,MODEL_MOVE_LEFT,MODEL_MOVE_RIGHT,GAMMAPLUS,GAMMAMINUS
    };
    void Close_();
    bool isOpend = false;
    explicit ModelSetDialog(QWidget *parent = 0);

    HalconCpp::HObject  ho_Image, ho_binImage, ho_Connections;
    HalconCpp::HObject  ho_DestRegions, ho_ImageRotate, ho_RegionMoved;

    // Local control variables
    HalconCpp::HTuple  hv_WindowID, hv_Area, hv_Row, hv_Column, hv_Row_Moved, hv_Column_Moved;
    HalconCpp::HTuple  hv_phi, hv_HomMat2DIdentity, hv_HomMat2DRotate;

    bool MirrorX = false;
    bool MirrorY = false;

    void TransRegion(bool isPaint = false);
    void OnTimer();

    ~ModelSetDialog();
    cv::Mat Snap_Image;

    bool isDraw = false;

    int Operation = NOP;

    bool eventFilter(QObject* obj,QEvent *event);

    bool Load_Model(std::string file_name);

    void CamSnap();

    void on_pushButton_clicked();

    void on_pushButton_GenCircle_clicked();

    void on_pushButton_GenRect_clicked();

    void on_pushButton_SaveImage_clicked();

    void on_pushButton_Grab_clicked();

   // void on_pushButton_SaveCalbi_clicked();

private slots:
  //  void on_pushButton_StartCabli_clicked();

    void on_pushButton_OSK_clicked();

    void on_pushButton_CopyStart_clicked();

    void on_pushButton_MirrorLR_clicked();

    void on_pushButton_MirrorUD_clicked();

    void on_pushButton_SaveImage_2_clicked();
private:
    Ui::ModelSetDialog *ui;
};

#endif // MODELSETDIALOG_H
