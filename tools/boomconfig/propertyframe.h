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
    void setEditable( bool value );
    QString getValueText();

signals:
    void valueUpdated(const QString& valueStr);

private slots:
    void on_pushButton_2_clicked();

    void on_lineEdit_selectionChanged();

    void on_lineEdit_textEdited(const QString &arg1);

private:
    Ui::PropertyFrame *ui;
    QString _value;
    bool _editable;

};

#endif // PROPERTYFRAME_H
