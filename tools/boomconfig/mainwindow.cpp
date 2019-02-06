#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../mcu/tx/frame.h"

#include <QMessageBox>
#include <QDebug>

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
    if( _isConnected )
    {
        Frame pingFrame = createPingFrame();
        int bsize = frameToBuffer( pingFrame, buffer, 512 );
        qDebug() << "bsize=" << bsize;
        _serial->write( (const char*)buffer, bsize );
    }
}

