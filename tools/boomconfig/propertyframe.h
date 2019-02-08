#ifndef PROPERTYFRAME_H
#define PROPERTYFRAME_H

#include <QFrame>

namespace Ui {
class PropertyFrame;
}

class PropertyFrame : public QFrame
{
    Q_OBJECT

public:
    explicit PropertyFrame(QWidget *parent = 0);
    ~PropertyFrame();

    void setLabelText( QString txt );
    void setValueText( QString txt );

signals:
    void valueUpdated(const QString& valueStr);

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::PropertyFrame *ui;
};

#endif // PROPERTYFRAME_H
