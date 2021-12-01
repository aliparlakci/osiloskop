#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ui_mainwindow.h>
#include <QDialog>
#include <QSerialPort>
#include <QByteArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonConnect_pressed();
    void on_pushButtonStatus_pressed();

    // -- Connection -- //
    void listPorts();     // restore & list com adresses

    // -- Graphing Slots -- //
    void setPlot();
    void makePlot();
    void updatePlot();
    void updateTimer(double timerScale);


    void on_dialHorizontalScale_valueChanged(int value);

    void on_dialVerticalScale_valueChanged(int value);



    void on_dialVeryicalShift_valueChanged(int value);

private:
    QSerialPort *serial;
    Ui::MainWindow *ui;
    QTimer *dataTimer;
};
#endif // MAINWINDOW_H
