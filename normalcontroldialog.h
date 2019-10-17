#ifndef NORMALCONTROLDIALOG_H
#define NORMALCONTROLDIALOG_H

#include <QDialog>

namespace Ui {
class NormalControlDialog;
}

class NormalControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NormalControlDialog(QWidget *parent = 0);
    ~NormalControlDialog();

private slots:
    void on_pushButton_LoadDxf_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_Save_clicked();

    void on_pushButton_Switch_clicked();

private:
    Ui::NormalControlDialog *ui;
};

#endif // NORMALCONTROLDIALOG_H
