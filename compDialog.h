/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/

#ifndef COMPDIALOG_H
#define COMPDIALOG_H

//---------------------------------------------------------------------------

#include "mainwindow.h"


//extern QObject *area;

//********************************************************************************
class CompasClass : public QObject
{
    Q_OBJECT
public:
    explicit CompasClass(QObject *parent = nullptr, float ugol = 0.0) : QObject(parent)
    {
        az = ugol;
    }
    ~CompasClass() {}

    Q_INVOKABLE float get_az() const { return az; }

    void set_pos(float ugol) { az = ugol; }

private:
    float az = 0.0;
};

//********************************************************************************
//---------------------------------------------------------------------------
class compDialog;
class MainWindow;
//-----------------------------------------------------------------
namespace Ui {
    class compDialog;
}
//-----------------------------------------------------------------

class compDialog : public QDialog
{
    Q_OBJECT
public:
    explicit compDialog(QWidget *parent = nullptr);
    ~compDialog();
    QObject *area;

public slots:
    void updateValue(float);
    QObject *getArea();

//signals:
//    void sendToQml(float val);

private:
    Ui::compDialog *uic;
    QString st;
    float az, last_az;
    CompasClass *Azimuth;
    QObject *RootObj;
};

//-----------------------------------------------------------------


//---------------------------------------------------------------------------




#endif // COMPDIALOG_H
