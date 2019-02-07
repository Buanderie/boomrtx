#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../common/include/frame.h"
#include "../../common/include/frameparser.h"

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

    _serial = new QSerialPort(this);
    _settings = new SettingsDialog(this);

    _pingTimer = new QTimer(this);
    connect(_pingTimer, SIGNAL(timeout()), this, SLOT(serialPing()));
    _pingTimer->start(1000);

    connect(_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
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
}

void MainWindow::readData()
{
    const QByteArray data = _serial->readAll();
    qDebug() << data.toStdString().c_str();
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
        Frame pingFrame = createPongFrame( 0x77 );
        int bsize = frameToBuffer( pingFrame, buffer, 512 );
        qDebug() << "bsize=" << bsize;
        print_bytes( std::cerr, "pute", (const unsigned char*)buffer, bsize );
//        _serial->write( (const char*)buffer, bsize );

        // Parser test
        FrameParser fp;
//        buffer[ bsize - 1 ] = 0xff;
        for( int i = 0; i < bsize; ++i )
        {
            if( fp.addByte( buffer[i] ) )
            {
                qDebug() << "Succesfully decoded frame !" << endl;
            }
        }
    }
}

