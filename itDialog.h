/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/

#ifndef ITDIALOG_H
#define ITDIALOG_H

#include <QtWidgets/QDialog>
#include <QString>

class pwdDialog;

//-----------------------------------------------------------------
namespace Ui {
    class pwdDialog;
}
//-----------------------------------------------------------------

class pwdDialog : public QDialog
{
    Q_OBJECT
public:
    explicit pwdDialog(QWidget *parent = nullptr, QString = "", QString = "");
    ~pwdDialog();

signals:
    void DoneW(QString);

private:
    Ui::pwdDialog *uiw;
    QString buf;

public slots:
    void slotOk();

};

//-----------------------------------------------------------------

#endif // ITDIALOG_H
