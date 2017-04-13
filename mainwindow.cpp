#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialtest = new QSerialPort(this);
    msgBox = new QMessageBox(this);
    readtime = new QTimer(this);
    ui->SendButton->hide();
    ui->ReceiveButton->hide();
    ui->uartoutput->hide();
    ui->uartdevice->setText("/dev/ttymxc");
    this->uport = "";
}

MainWindow::~MainWindow()
{
    serialtest->close();
    delete ui;
}

void MainWindow::on_SubmitButton_clicked()
{
    /*get uart port device from uartdevice qlineedit*/
    this->uport = ui->uartdevice->text();
       if(this->uport != "")
       {
           serialtest->setPortName(QString(this->uport));
           serialtest->setBaudRate(QSerialPort::Baud115200);
           serialtest->setDataBits(QSerialPort::Data8);
           serialtest->setParity(QSerialPort::NoParity);
           serialtest->setStopBits(QSerialPort::OneStop);
           serialtest->setFlowControl(QSerialPort::NoFlowControl);

           this->fd = serialtest->open(QIODevice::ReadWrite);


           if(this->fd == false)
           {
               ui->label->setText("uart port open error!");
           }
           else{
                  ui->label->setText("uart setup error!");
                  /*modify lable output and show send/receive button and uartoutput line edit*/
                  ui->label->setText("uart setup successfully! Press Send or Receive Button");
                  ui->SendButton->show();
                  ui->ReceiveButton->show();
                  ui->uartoutput->show();
                  ui->uartoutput->setText("input string for send here or default message will be sent");

            }
       }
}

void MainWindow::on_SendButton_clicked()
{
    /*capture uartoutput content QString and change to char[]*/
        this->len = sprintf(this->data_send, ui->uartoutput->text().toLocal8Bit().constData());
        /*judge if uartoutput lineedit is blank*/
        if(this->len == 0)
        {
            /*config default message for sending once lineedit is blank*/
            this->len = sprintf(this->data_send,"this is a test program");
            this->data_send[this->len]='\0';
            /*popup message box to choose*/
            msgBox->setText("iput line is blank, default message as below will be sent");
            msgBox->setInformativeText("this is a test program");
            msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox->setDefaultButton(QMessageBox::Yes);
            if(msgBox->exec() == QMessageBox::Yes)
            {
                /*send default message*/
                this->ch = serialtest->write(this->data_send,this->len);
                if(this->ch == -1)
                {
                    QMessageBox::warning(NULL, "warning", "send error!");
                }
                else
                    ui->label->setText("send default message successfully!");
            }
        }
        else
        {
            /*send input message*/
            if(this->len != -1)
            {
                this->ch = serialtest->write(this->data_send,this->len);
                if(this->ch == -1)
                {
                    QMessageBox::warning(NULL, "warning", "send error!");
                }
                else
                    ui->label->setText("send input message successfully!");
            }
            else
                QMessageBox::warning(NULL, "warning", "error set data for sending!");
        }
}

void MainWindow::uartread()
{
    /* stop the timer for read timeout*/
    if(readtime->isActive())
        readtime->stop();
    /*start to read from uart port*/
    this->data_read = serialtest->readAll();
    /*check if there is read error*/
    if (serialtest->error() == QSerialPort::ReadError) {
        QMessageBox::warning(NULL, "warning", "Failed to read!");

    }else{
        /* convert data_read from QByteArray to QString and check if "ENTER" is received*/
        QString temp = QString::fromStdString(this->data_read.toStdString());
        if (temp != "\r")
        {
            this->str += temp;
        }else{
        /* output data received to uartoutput lineeidt */
            ui->uartoutput->setText(this->str);
            ui->label->setText("receiving finished!");
            QObject::disconnect(serialtest,SIGNAL(readyRead()),this,SLOT(uartread()));
        }
    }
}

void MainWindow::on_ReceiveButton_clicked()
{
    this->str = "";
    /* create messagebox */
    msgBox->setText("Received data will show in edit line");
    msgBox->setInformativeText("press ENTER when finishing, 5s timeout for no receiving");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::Yes);
    /* if Yes button clicked */
    if(msgBox->exec() == QMessageBox::Yes)
    {
        /*ready to read from uart device */
        QObject::connect(serialtest,SIGNAL(readyRead()),this,SLOT(uartread()));
        /* set 5s timeout time */
        ui->label->setText("ready for receive!");
        QObject::connect(readtime,SIGNAL(timeout()),this,SLOT(readtimeout()));
        readtime->start(5000);
    }
}

void MainWindow::readtimeout()
{
    /* stop timer */
    readtime->stop();
    /*stop reading */
    QObject::disconnect(serialtest,SIGNAL(readyRead()),this,SLOT(uartread()));
    QMessageBox::warning(NULL, "warning", "data read timeout!");
}
