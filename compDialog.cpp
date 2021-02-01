/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/


#include "compDialog.h"
#include "ui_compDialog.h"

//--------------------------------------------------------------------------------

QObject *area;

//--------------------------------------------------------------------------------
compDialog::compDialog(QWidget *parent) : QDialog(parent), uic(new Ui::compDialog)
{
    uic->setupUi(this);

    az = last_az = 0.0;

    uic->quickWidget->setSource(QUrl::fromLocalFile("./compass.qml"));
    Azimuth = new CompasClass(uic->quickWidget, az);
    if (Azimuth) uic->quickWidget->rootContext()->setContextProperty("area", Azimuth);

    this->setWindowFlags(Qt::Window);

    uic->label->setText("Азимут : ");
    st.clear();
}
//--------------------------------------------------------------------------------
QObject *compDialog::getArea()
{
    return (static_cast<QObject *>(uic->quickWidget));
}
//--------------------------------------------------------------------------------
compDialog::~compDialog()
{
    if (Azimuth) delete Azimuth;
    delete uic;
}
//--------------------------------------------------------------------------------
void compDialog::updateValue(float val)
{
    az = val;
    if (az != last_az) {
        last_az = az;
        if (Azimuth) Azimuth->set_pos(az);

        st.sprintf("Азимут : %.2f deg.",val);
        uic->label->setText(st);
    }

}
//--------------------------------------------------------------------------------

