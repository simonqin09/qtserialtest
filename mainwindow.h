#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPort>
#include <QTimer>

namespace Ui {
class MainWindow;
class QLineEdit;
class QLable;
class QPushButton;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_SubmitButton_clicked();

    void on_SendButton_clicked();

    void on_ReceiveButton_clicked();
    void uartread();
    void readtimeout();


private:
    Ui::MainWindow *ui;
    Ui::QLineEdit *uartdevice;
    Ui::QLineEdit *uartoutput;
    Ui::QLable *lable;
    Ui::QPushButton *SendButton;
    Ui::QPushButton *ReceiveButton;
    QMessageBox *msgBox;
    QSerialPort *serialtest;
    QTimer *readtime;

    QString uport;
    QString str;
    QByteArray data_read;

    bool fd;
    int len, ch;
    char data_send[1024];
};

#endif // MAINWINDOW_H
