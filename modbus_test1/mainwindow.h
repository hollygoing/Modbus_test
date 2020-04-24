#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mcmodbusrtu.h"
typedef union{
            int mint;
            float mfloat;
            unsigned short mshort[2];
            unsigned char mchar[4];
        }ShortToOther;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void McModbusRTUMessage(unsigned char mID,int mFunction,QByteArray Data);

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    McModbusRTU *mmrtu;
};

#endif // MAINWINDOW_H
