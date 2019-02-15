#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../common/frame.h"
#include "../../common/frameparser.h"
#include "../../common/protocol.h"

#include <QMessageBox>
#include <QDebug>

#include <iomanip>
#include <iostream>
using namespace std;

void print_bytes(std::ostream& out, const char *title, const unsigned char *data, size_t dataLen, bool format = true, int symbol_per_line = 64 ) {
    out << title << std::endl;
    out << std::setfill('0');
    for(size_t i = 0; i < dataLen; ++i) {
        out << std::hex << std::setw(2) << (int)data[i];
        if (format) {
            out << (((i + 1) % symbol_per_line == 0) ? "\n" : " ");
        }
    }
    out << std::endl;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Load BoomRTX logo
    QPixmap mypix (":/logo_small.png");
    ui->logoLabel->setPixmap(mypix);

    _isConnected = false;
    notifySerialLinkConnection( _isConnected );

    _serial = new QSerialPort(this);
    _settings = new SettingsDialog(this);

    _pingTimer = new QTimer(this);
    connect(_pingTimer, SIGNAL(timeout()), this, SLOT(serialPing()));
    _pingTimer->start(50);

    _radioQualityTimer = new QTimer(this);
    connect(_radioQualityTimer, SIGNAL(timeout()), this, SLOT(radioQualityRequest()));

    connect(_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    _parser = (void*)(new FrameParser());

    // Quality progress bar
    ui->qualityValue->setDecimals( 0 );
    ui->qualityValue->setRange( 0.0, 1.0 );
    ui->qualityValue->setValue( 0.33 );
    ui->qualityValue->setBarStyle( QRoundProgressBar::StyleLine );
    ui->qualityValue->setOutlinePenWidth(18);
    ui->qualityValue->setDataPenWidth(10);
    /*
    ui->signalProgressBar->setDecimals(2);
    ui->signalProgressBar->setBarStyle(QRoundProgressBar::StyleLine);
    ui->signalProgressBar->setOutlinePenWidth(18);
    ui->signalProgressBar->setDataPenWidth(10);
    */

    // Properties
    ui->deviceId->setLabelText( "Device ID\n[0 - 255]" );
    ui->deviceType->setLabelText( "Device Type" );
    ui->deviceType->setEditable( false );
    ui->deviceTxPower->setLabelText( "Radio TX Power\n[1 - 8]" );
    ui->deviceChannel->setLabelText( "Radio Channel\n[1 - 127]" );

    ui->deviceId->setValueText("-");
    ui->deviceType->setValueText("-");
    ui->deviceTxPower->setValueText("-");
    ui->deviceChannel->setValueText("-");

    // Transmitter settings
    ui->targetDevice1->setLabelText("Target Device #1 ID");
    ui->targetDevice2->setLabelText("Target Device #2 ID");
    ui->transmitterSettings->hide();

    _deviceId = -1;
    frameIdx = 0;

    // Properties connections
    connect(ui->deviceId, &PropertyFrame::valueUpdated, this, &MainWindow::onDeviceIDUpdate);
    connect(ui->deviceChannel, &PropertyFrame::valueUpdated, this, &MainWindow::onDeviceChannelUpdate);
    connect(ui->deviceTxPower, &PropertyFrame::valueUpdated, this, &MainWindow::onDevicePowerUpdate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Serial Settings dialog
void MainWindow::on_actionConnect_triggered()
{
    _settings->setWindowModality(Qt::WindowModal);
    _settings->exec();

    const SettingsDialog::Settings p = _settings->settings();
    _serial->setPortName(p.name);
    _serial->setBaudRate(p.baudRate);
    _serial->setDataBits(p.dataBits);
    _serial->setParity(p.parity);
    _serial->setStopBits(p.stopBits);
    _serial->setFlowControl(p.flowControl);

    if( _serial->isOpen() )
    {
        _isConnected = false;
        _serial->close();
    }

    if (_serial->open(QIODevice::ReadWrite))
    {
        _isConnected = true;
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), _serial->errorString());
        _isConnected = false;
    }
    notifySerialLinkConnection( _isConnected );

}

void MainWindow::readData()
{
    const QByteArray data = _serial->readAll();

    print_bytes( std::cerr, "data", (const unsigned char*)(data.constData()), data.size() );

    FrameParser * fp = (FrameParser*)_parser;
    for( int k = 0; k < data.size(); ++k )
    {
        if( fp->addByte(data.at(k) ) )
        {
            qDebug() << "RECEIVED VALIDFRAME!" << frameIdx++;
            Frame ff = fp->getFrame();
            //            qDebug() << "OPCODE = " << (int)ff.opcode;
            processFrame( ff.opcode, ff.payload, ff.payload_size );
        }
    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{

}

void MainWindow::serialPing()
{
    uint8_t buffer[ 512 ];
    // if( _isConnected )
    {
        //        Frame pingFrame = createPongFrame( (uint8_t)(rand() % 256) );
        Frame pingFrame = createPingFrame();
        int bsize = frameToBuffer( pingFrame, buffer, 512 );
        if( _isConnected )
        {
            if( _serial->isWritable() )
            {
                _serial->write( (const char*)buffer, bsize );
            }
            _qual.pushPing();
            double quality = _qual.quality();
            // ui->qualityValue->setText( QString::number( quality * 100.0, 10, 1 ) + QString("%") );
            ui->qualityValue->setValue( quality );
            if( quality > 0.85 )
            {
                qDebug() << "@@@@ " << ui->deviceType->getValueText();
                notifySerialLinkQuality(true);
                if( ui->deviceType->getValueText() == "TRANSMITTER" )
                {
                    if( !ui->transmitterSettings->isVisible() )
                    {
                        ui->transmitterSettings->show();
                        _radioQualityTimer->start(50);
                        // Also request targets...
                        // TODO
                    }
                }
                else {
                    ui->transmitterSettings->hide();
                    _radioQualityTimer->stop();
                }
            }
            else
            {
                notifySerialLinkQuality(false);
                ui->transmitterSettings->hide();
            }
        }
    }
}

void MainWindow::radioQualityRequest()
{
    qDebug() << "ASKING FOR RADIO QUALITY !";
    uint8_t buffer[ 512 ];
    Frame getRadioQualFrame = createGetRadioQualityFrame( (uint8_t)_deviceId );
    int bsize = frameToBuffer( getRadioQualFrame, buffer, 512 );
    if( _isConnected )
    {
        if( _serial->isWritable() )
        {
            _serial->write( (const char*)buffer, bsize );
        }
    }
}

void MainWindow::notifySerialLinkConnection(bool value)
{
    ui->serialLinkLed->setState(value);
    if( value )
    {
        ui->serialLinkLabel->setText( tr("Connected") );
    }
    else
    {
        ui->serialLinkLabel->setText( tr("Not Connected") );
    }
}

void MainWindow::notifySerialLinkQuality(bool value)
{
    ui->serialQualityLed->setState( value );
}

void MainWindow::processFrame(int opcode, uint8_t *payload, size_t payload_size )
{
    print_bytes( std::cerr, "payload", (const unsigned char*)(payload), payload_size );

    switch( opcode )
    {

    case OP_PONG: // PONG
    {
        qDebug() << "Received PONG from device ID=" << (int)payload[0];
        _qual.pushPong();
        uint8_t device_type = payload[ 1 ];
        QString deviceTypeStr = tr("UNKWOWN");
        if( device_type == 0x00 )
        {
            deviceTypeStr = tr("TRANSMITTER");
        }
        else if( device_type == 0x01 )
        {
            deviceTypeStr = tr("RECEIVER");
        }

        if( (int)payload[0] != _deviceId )
        {
            // Then, update device id...
            _deviceId = (int)payload[0];
            qDebug() << "Detected DEVICE ID=" << _deviceId;
            ui->deviceId->setValueText( QString::number( _deviceId, 16 ).toUpper() );
            requestRadioChannel();
            requestRadioPower();
        }

        ui->deviceType->setValueText(deviceTypeStr);
        break;
    }

    case OP_RADIO_CHANNEL_ACK:
    {
        qDebug() << "Received RADIO_CHANNEL from device ID=" << (int)payload[0];
        uint8_t device_id = payload[ 0 ];
        uint8_t radio_channel = payload[ 1 ];
        qDebug() << "******* RADIO_CHANNEL = " << (int)radio_channel;
        ui->deviceChannel->setValueText( QString::number( radio_channel, 16 ).toUpper() );
        break;
    }

    case OP_RADIO_POWER_ACK:
    {
        qDebug() << "Received RADIO_POWER from device ID=" << (int)payload[0];
        uint8_t device_id = payload[ 0 ];
        uint8_t radio_power = payload[ 1 ];
        qDebug() << "******* RADIO_POWER = " << (int)radio_power;
        ui->deviceTxPower->setValueText( QString::number( radio_power, 16 ).toUpper() );
        break;
    }

    case OP_RADIO_QUALITY_ACK:
    {
        qDebug() << "Received RADIO_QUALITY from device ID=" << (int)payload[0];
        uint8_t device_id = payload[ 0 ];
        uint8_t radio_quality = payload[ 1 ];
        double radioQuality = (double)radio_quality / 255.0;
        qDebug() << "******* RADIO_QUALITY = " << radioQuality;
        ui->radioQualityProgressBar->setValue( radioQuality * 100 );
        break;
    }

    default:
        break;

    }
}

void MainWindow::requestRadioChannel()
{
    uint8_t buffer[ 512 ];
    Frame radioChannelFrame = createGetRadioChannelFrame( _deviceId );
    int bsize = frameToBuffer( radioChannelFrame, buffer, 512 );
    if( _serial->isWritable() )
    {
        _serial->write( (const char*)buffer, bsize );
    }
}

void MainWindow::requestRadioPower()
{
    uint8_t buffer[ 512 ];
    Frame radioPowerFrame = createGetRadioPowerFrame( _deviceId );
    int bsize = frameToBuffer( radioPowerFrame, buffer, 512 );
    if( _serial->isWritable() )
    {
        _serial->write( (const char*)buffer, bsize );
    }
}

void MainWindow::setDeviceId( uint8_t deviceId )
{
    uint8_t buffer[ 512 ];
    Frame deviceIdFrame = createSetDeviceIdFrame( deviceId );
    int bsize = frameToBuffer( deviceIdFrame, buffer, 512 );
    if( _serial->isWritable() )
    {
        _serial->write( (const char*)buffer, bsize );
    }
}

void MainWindow::setDeviceChannel(uint8_t deviceChannel)
{
    uint8_t buffer[ 512 ];
    Frame deviceChannelFrame = createSetDeviceChannelFrame( _deviceId, deviceChannel );
    int bsize = frameToBuffer( deviceChannelFrame, buffer, 512 );
    if( _serial->isWritable() )
    {
        _serial->write( (const char*)buffer, bsize );
    }
}

void MainWindow::setDevicePower(uint8_t devicePower)
{
    uint8_t buffer[ 512 ];
    Frame devicePowerFrame = createSetDevicePowerFrame( _deviceId, devicePower );
    int bsize = frameToBuffer( devicePowerFrame, buffer, 512 );
    if( _serial->isWritable() )
    {
        _serial->write( (const char*)buffer, bsize );
    }
}

void MainWindow::on_actionDisconnect_triggered()
{
    _isConnected = false;
    _serial->close();
    notifySerialLinkConnection(_isConnected);
}

void MainWindow::onDeviceIDUpdate(const QString &valueStr)
{
    qDebug() << "Need to update DEVICE_ID";
    int pol = valueStr.toInt(nullptr, 16);
    setDeviceId( pol );
}

void MainWindow::onDeviceChannelUpdate(const QString &valueStr)
{
    qDebug() << "Need to update DEVICE_CHANNEL";
    int pol = valueStr.toInt(nullptr, 16);
    setDeviceChannel( pol );
}

void MainWindow::onDevicePowerUpdate(const QString &valueStr)
{
    qDebug() << "Need to update DEVICE_POWER";
    int pol = valueStr.toInt(nullptr, 16);
    setDevicePower( pol );
}
