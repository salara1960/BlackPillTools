/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/

#include "defs.h"
#include "graf.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QChartView>


//------------------------------------------------------------------------------------------
//   Конструктор класса для прорисовки графика
//
Chart::Chart(QGraphicsItem *parent, Qt::WindowFlags wFlags, int tp):
            QChart(QChart::ChartTypeCartesian, parent, wFlags),
            cser(0),
            axX(new QValueAxis()),
            axY(new QValueAxis()),
            step(2),//1
            cor_x(0),//5
            cor_y(0)//1
{

    type = tp;

    titStr.resize(gNone);
    sufix << " mmHg" << " %" << " ℃" << " V" << " ℃" << " ℃";
    for (int i = 0; i < gNone; i++) titStr[i] = "";

    cser = new QSplineSeries(this);

    connect(this, &Chart::sig_newTitle, this, &Chart::slot_newTitle);

}
//------------------------------------------------------------------------------------------
//   Диструктор класса
//
Chart::~Chart()
{
}
//------------------------------------------------------------------------------------------
//   Метод устанавливает атрибуты для графика в зависимости от его типа
//    (давление, скорость звука, температура, напряжение питания)
//
void Chart::setConf(const chart_conf_t *conf)
{

    QPen color(Qt::white);

    switch (conf->type) {
        case gDeep ://давление
            this->setTheme(ChartThemeDark);
            color = QPen(Qt::white);
        break;
        case gHumi ://
            this->setTheme(ChartThemeBlueCerulean);
            color = QPen(Qt::yellow);
        break;
        case gTemp ://температура
            this->setTheme(ChartThemeLight);
            color = QPen(Qt::blue);
        break;
        case gTemp2 ://температура
            this->setTheme(ChartThemeLight);
            color = QPen(Qt::magenta);
        break;
        case gTemp3 ://температура
            this->setTheme(ChartThemeLight);
            color = QPen(Qt::cyan);
        break;
        case gVolt ://напряжение питания
            this->setTheme(ChartThemeBrownSand);
            color = QPen(Qt::red);
        break;
    }
    titStr[type] = conf->name;

    this->setTitle(titStr[type]);

    color.setWidth(2);
    cser->setPen(color);

    cor_x = conf->cx;
    cor_y = conf->cy;
    step = conf->step;
    rx = conf->rx;
    ry = conf->ry;
    tik = conf->tik;

    cser->append(cor_x, cor_y);
    addSeries(cser);
    addAxis(axX, Qt::AlignBottom);
    addAxis(axY, Qt::AlignLeft);
    cser->attachAxis(axX);
    cser->attachAxis(axY);
    axX->setTickCount(tik);//5
    axX->setRange(rx.min, rx.max);//0, 10
    axY->setRange(ry.min, ry.max);//-5, 10
}
//------------------------------------------------------------------------------------------
//   Метод-слот прорисовывает график по принятому значению и при необходимости
//   корректирует мин. и макс. значения координатной оси Y
//
void Chart::slot_newData(float val, void *pic)
{

    if (reinterpret_cast<Chart *>(pic) != this) return;

    if (val >= (ry.max - step*2)) {//(ry.max - step)
        ry.max = val + step;
        axY->setMax(ry.max);
    } else {
        if ((val + step*4) < ry.max) {
            ry.max = val + step*3;
            axY->setMax(ry.max);
        }
    }
    if (val <= (ry.min + step*2)) {//(ry.min + step)
        ry.min = val - step;
        axY->setMin(ry.min);
    } else {
        if ((val - step*4) > ry.min) {
            ry.min = val - step*3;
            axY->setMin(ry.min);
        }
    }
    qreal x = plotArea().width() / axX->tickCount();
    qreal y = (axX->max() - axX->min()) / axX->tickCount();
    cor_x += y;
    cor_y = val;
    cser->append(cor_x, cor_y);
    scroll(x, 0);

    this->setTitle(titStr[type] + " : " + QString::number(val, 'f', 2) + sufix.at(type));

}
//------------------------------------------------------------------------------------------
//   Метод-слот обновляет титульный заголовок графика
//
void Chart::slot_newTitle(const QString title, void *pic)
{
    if (reinterpret_cast<Chart *>(pic) == this) {
        this->setTitle(title);
    }
}
//------------------------------------------------------------------------------------------

