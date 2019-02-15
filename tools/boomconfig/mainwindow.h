#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>

#include "settingsdialog.h"
#include "../../common/linkquality.h"

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
    void radioQualityRequest();

    void on_actionDisconnect_triggered();

    void onDeviceIDUpdate( const QString& valueStr );
    void onDeviceChannelUpdate( const QString& valueStr );
    void onDevicePowerUpdate( const QString& valueStr );

private:
    Ui::MainWindow *ui;

    bool _isConnected;
    QSerialPort * _serial;
    SettingsDialog *_settings = nullptr;
    void notifySerialLinkConnection( bool value );
    void notifySerialLinkQuality( bool value );

    QTimer * _pingTimer = nullptr;
    void* _parser;
    int frameIdx;

    // Device information
    int _deviceId;

    // PING/PONG stats
    LinkQuality<50> _qual;

    // Transmitter Radio Quality Timer
    QTimer * _radioQualityTimer = nullptr;

    void processFrame(int opcode, uint8_t* payload , size_t payload_size);
    void requestRadioChannel();
    void requestRadioPower();

    void setDeviceId(uint8_t deviceId);
    void setDeviceChannel(uint8_t deviceChannel);
    void setDevicePower(uint8_t devicePower);
};

#endif // MAINWINDOW_H
