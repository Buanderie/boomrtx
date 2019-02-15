#include "propertyframe.h"
#include "ui_propertyframe.h"

PropertyFrame::PropertyFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PropertyFrame)
{
    ui->setupUi(this);
    setEditable( true );
}

PropertyFrame::~PropertyFrame()
{
    delete ui;
}

void PropertyFrame::setLabelText(QString txt)
{
    ui->label->setText( txt );
}

void PropertyFrame::setValueText(QString txt)
{
    _value = txt;
    if( _editable )
    {
        ui->lineEdit->setText( _value );
    }
    else
    {
        ui->valueLabel->setText( _value );
    }
}

void PropertyFrame::setEditable(bool value)
{
    _editable = value;
    if( _editable )
    {
        ui->lineEdit->show();
        ui->valueLabel->hide();
    }
    else
    {
        ui->lineEdit->hide();
        ui->valueLabel->show();
        ui->pushButton_2->hide();
    }
}

QString PropertyFrame::getValueText()
{
    return _value;
}

void PropertyFrame::on_pushButton_2_clicked()
{
    emit valueUpdated(_value);
}
