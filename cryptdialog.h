#ifndef CRYPTDIALOG_H
#define CRYPTDIALOG_H

#include <QDialog>

namespace Ui {
class CryptDialog;
}

class CryptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CryptDialog(char* Cyrpt = nullptr,QWidget *parent = 0);
    ~CryptDialog();
    char **m_Crypt = nullptr;
    bool Init();
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
private:
    Ui::CryptDialog *ui;
};

#endif // CRYPTDIALOG_H
