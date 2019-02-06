#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>

#include "settingsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionConnect_triggered();

    /*
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    */

    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void serialPing();

private:
    Ui::MainWindow *ui;

    bool _isConnected;
    QSerialPort * _serial;
    SettingsDialog *_settings = nullptr;

    QTimer * _pingTimer = nullptr;

};

#endif // MAINWINDOW_H
