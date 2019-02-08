#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>

#include "settingsdialog.h"
#include "linkquality.h"

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

    void on_actionDisconnect_triggered();

    void onDeviceIDUpdate( QString& valueStr );

private:
    Ui::MainWindow *ui;

    bool _isConnected;
    QSerialPort * _serial;
    SettingsDialog *_settings = nullptr;
    void notifySerialLinkConnection( bool value );
    void notifySerialLinkQuality( bool value );

    QTimer * _pingTimer = nullptr;
    void* _parser;

    // PING/PONG stats
    LinkQuality _qual;

    void processFrame(int opcode, uint8_t* payload , size_t payload_size);

};

#endif // MAINWINDOW_H
