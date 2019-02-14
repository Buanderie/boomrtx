#include "propertyframe.h"
#include "ui_propertyframe.h"

PropertyFrame::PropertyFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PropertyFrame)
{
    ui->setupUi(this);
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
    ui->lineEdit->setText( txt );
}

QString PropertyFrame::getValueText()
{
    return ui->lineEdit->text();
}

void PropertyFrame::on_pushButton_2_clicked()
{
    emit valueUpdated(ui->lineEdit->text());
}
