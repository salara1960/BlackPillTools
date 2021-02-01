/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/


#include "itDialog.h"
#include "ui_itDialog.h"


//**************************************************************************************
//   Конструктор класса окна
//
pwdDialog::pwdDialog(QWidget *parent, QString title, QString arr) : QDialog(parent), uiw(new Ui::pwdDialog)
{
    uiw->setupUi(this);

    this->setWindowIcon(QIcon("png/button.png"));
    this->setFixedSize(this->size());

    setWindowTitle(title);

    buf.clear();
    uiw->line->setText(arr);

    connect(uiw->but_ok, SIGNAL(pressed()), this, SLOT(slotOk()));

}
//-----------------------------------------------
//   Диструктор класса
//
pwdDialog::~pwdDialog()
{
    this->disconnect();
    delete uiw;
}
//-----------------------------------------------
//   Метод-слот обработки сигнала (нажатие кнопки)
//
void pwdDialog::slotOk()
{
    buf.append(uiw->line->text().toLocal8Bit());

    // сигнал основному окну
    emit DoneW(buf);
}
//**************************************************************************************

