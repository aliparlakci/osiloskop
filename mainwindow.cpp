#include "mainwindow.h"
#include "ui_mainwindow.h"

//    External libs    //
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QObject>
#include <QTimer>


// connecting controller //
bool isConnected = false;
bool dataAcq = false;


//Global Parameters
int vertical = 0;
int verticalMax;
int verticalMin;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // setup //
    ui->setupUi(this);
    dataTimer = new QTimer(this);    //Timer Setup
    serial = new QSerialPort(this);    // Serial Setup


    //-----code for configuring serial port conenctions-----//
    listPorts();
    setPlot();

}


// ----- Closing Application ------ //
MainWindow::~MainWindow()
{
    if(serial->isOpen())
    {
        serial->close();
    }
    delete ui;
}

// ---- Functions ---- //

void MainWindow::listPorts()    // lists available ports *WIP//
{
    // resetting boxes
    ui->comboBoxPort->clear();
    ui->comboBoxBaud->clear();

    // adding available serial ports
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->comboBoxPort->addItem(serialPortInfo.portName());
    }

    // Adding baud rates
    QList<qint32> baudrates = QSerialPortInfo::standardBaudRates();
    foreach (qint32 baudrate, baudrates)
    {
        ui->comboBoxBaud->addItem(QString::number(baudrate));
    }
    // insert code below for adding custom baud rates
    ui->comboBoxBaud->addItem("250000");

    // you can modify code below to set default baudrate
    ui->comboBoxBaud->setCurrentText("250000");
}

// ------- Buttons --------- //
void MainWindow::on_pushButtonConnect_pressed()
{
    if(!isConnected)
    {
        serial->setPortName(ui->comboBoxPort->currentText());
        serial->open(QIODevice::ReadWrite);
        serial->setBaudRate(ui->comboBoxBaud->currentText().toInt());
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        while(!serial->isOpen()) serial->open(QIODevice::ReadWrite);
        if (serial->isOpen() && serial->isWritable())
        {
            qDebug() << "Serial Connection is available" << endl;
            ui->pushButtonConnect->setText("Disconnect");
            isConnected = true;
        }
        else
        {
            qDebug() << "Something went wrong !!" << endl;
        }
    }
    else if(isConnected)
    {
        serial->close();
        qDebug() << "Disconnected!" << endl;
        ui->pushButtonConnect->setText("Connect");
        isConnected = false;
    }
}

void MainWindow::on_pushButtonStatus_pressed()
{
    if(isConnected)
    {
        dataAcq = true;
        //makePlot();
        /*serial->write("1");
        while(true)
        {
            QString input = serial->readLine();
            if(input == "#comok")
            {
                serial->write("#start\n");
                ui->labelStatus->setText("Connection is okay !");
                break;
            }
            else
            {
                ui->labelStatus->setFixedHeight(30);
                //ui->labelStatus->setText("There is problem at serial \n connection ERR_#2");
                ui->labelStatus->setText(input);
                break;
            }
        }*/
    }
    else
    {
        ui->labelStatus->setText("Not Connected ERR_#1");
    }
}

// ------ Graph Functions ------ //
void MainWindow::setPlot(){             // Initalize paremeters of graph
    // give the axes some labels:
    ui->customPlot->xAxis->label();
    ui->customPlot->xAxis->setTickLabels(false);
    //ui->customPlot->yAxis->setLabel("y");
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);
    ui->customPlot->yAxis->setRange(verticalMin, verticalMax);
    ui->customPlot->yAxis->ticker()->setTickCount(10);
}

void MainWindow::updatePlot()
{
    ui->customPlot->yAxis->setRange(verticalMin, verticalMax);
}

void MainWindow::makePlot()            // Draw signal on graph
{
    QString serialBuffer = "";
    // generate some data:
    QByteArray SerialInput = serial->readLine();
    serialBuffer+= QString::fromStdString(SerialInput.toStdString());
    QStringList input = serialBuffer.split(",");
    qDebug() << input.length() << endl;

    QVector<double> x(input.length()), y(input.length()), trg;
    // initialize with entries 0..100
    for (int i=0; i<input.length(); ++i)
    {
      x[i] = i; // x goes from -1 to 1
      y[i] = input[i].toDouble() + vertical; // let's plot a quadratic function
    }
    // create graph and assign data to it:
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x, y);
    ui->customPlot->graph(1)-> setData(x,trg);


    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(-1, y.length());
    ui->customPlot->replot();
}

// Updatescale Horizontal ?
void MainWindow::updateTimer(double timerScale)
{
    if(!dataTimer->isActive())
    {
        QObject::connect(dataTimer, SIGNAL(timeout()), this, SLOT(makePlot()));
        dataTimer->start(timerScale);
    }
    else
    {
        qDebug() << "Updated value" << endl;
        dataTimer->start(timerScale);
    }

}

// --- Graph Controls --- //
// Horizontal Scale
void MainWindow::on_dialHorizontalScale_valueChanged(int value)
{
    if(isConnected)
    {
       updateTimer(value/100);
    }
}

// Vertical Scale
void MainWindow::on_dialVerticalScale_valueChanged(int value)
{
    int c = 0;
    qDebug() << vertical << endl;
    if(value <= 3 )
    {
        if(value == 1)
            c = 1;
        else if (value == 2)
            c = 2;
        else
            c = 5;

        verticalMax = 0 +  (5 * c);
        verticalMin = 0 -  (5 * c);
    }
    else if (value <= 6 )
    {
        if(value == 4 )
            c = 1;
        else if (value == 5)
            c = 2;
        else
            c = 5;

        verticalMax = 0 +  (50 * c);
        verticalMin = 0 -  (50 * c);
    }
    else if(value <= 9 )
    {
        if(value == 7)
            c = 1;
        else if (value == 8)
            c = 2;
        else
            c = 5;

        verticalMax = 0 +  (500 * c);
        verticalMin = 0 -  (500 * c);
    }
    else if (value <= 12)
    {
        if(value == 10)
            c = 1;
        else if (value == 11)
            c = 2;
        else
            c = 5;

        verticalMax = 0 + ( 5000 * c);
        verticalMin = 0 - ( 5000 * c);
    }
    else
    {
        verticalMax = 0 + ( 50000 * c);
        verticalMin = 0 - ( 50000 * c);
    }
    setPlot();
}

// Vertical Shift
void MainWindow::on_dialVeryicalShift_valueChanged(int value)
{
    vertical = value;
}

// Trigger Control









