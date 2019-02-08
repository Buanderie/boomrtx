#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../common/frame.h"
#include "../../common/frameparser.h"

#include <QMessageBox>
#include <QDebug>

#include <iomanip>
#include <iostream>

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
    _isConnected = false;
    notifySerialLinkConnection( _isConnected );

    _serial = new QSerialPort(this);
    _settings = new SettingsDialog(this);

    _pingTimer = new QTimer(this);
    connect(_pingTimer, SIGNAL(timeout()), this, SLOT(serialPing()));
    _pingTimer->start(200);

    connect(_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    _parser = (void*)(new FrameParser());

    // Properties
    ui->deviceId->setLabelText( "ID" );
    ui->deviceType->setLabelText( "Device Type" );
    ui->deviceTxPower->setLabelText( "Radio TX Power");
    ui->deviceChannel->setLabelText("Radio Channel\n[1 - 127]");

    ui->deviceId->setValueText("-");
    ui->deviceType->setValueText("-");
    ui->deviceTxPower->setValueText("-");
    ui->deviceChannel->setValueText("-");

    // Properties connections
    connect(ui->deviceId, &PropertyFrame::valueUpdated, this, &MainWindow::onDeviceIDUpdate);

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

    //    qDebug() << "DATA ??? ";
    //    qDebug() << "SIZE=" << data.size();

    FrameParser * fp = (FrameParser*)_parser;
    for( int k = 0; k < data.size(); ++k )
    {
        if( fp->addByte(data.at(k) ) )
        {
            //            qDebug() << "RECEIVED VALIDFRAME!";
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
            _serial->write( (const char*)buffer, bsize );
            _qual.pushPing();
            double quality = _qual.quality();
            ui->qualityValue->setText( QString::number( quality * 100.0, 10, 1 ) + QString("%") );
            if( quality > 0.95 )
            {
                notifySerialLinkQuality(true);
            }
            else
                notifySerialLinkQuality(false);
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
    switch( opcode )
    {

    case 0x01: // PONG
    {
        qDebug() << "Received PONG from device ID=" << (int)payload[0];
        _qual.pushPong();
        ui->deviceId->setValueText( QString::number( (int)payload[0], 16 ).toUpper() );
        uint8_t device_type = payload[ 1 ];
        QString deviceTypeStr = tr("UNKWOWN");
        if( device_type == 0x00 )
        {
            deviceTypeStr = tr("EMITTER");
        }
        else if( device_type == 0x01 )
        {
            deviceTypeStr = tr("RECEIVER");
        }
        ui->deviceType->setValueText(deviceTypeStr);
        break;
    }

    default:
        break;

    }
}


void MainWindow::on_actionDisconnect_triggered()
{
    _isConnected = false;
    _serial->close();
    notifySerialLinkConnection(_isConnected);
}

void MainWindow::onDeviceIDUpdate(QString &valueStr)
{
    qDebug() << "Need to update DEVICE_ID";
}
