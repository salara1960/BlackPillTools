/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/


#ifndef GRAF_H
#define GRAF_H

#include <QtCharts/QChart>
#include <QChartView>

//---------------------------------------------------------------------------

typedef struct {
    float min;
    float max;
} min_max_t;

typedef struct {
    QString name;
    float cx;
    float cy;
    min_max_t rx;
    min_max_t ry;
    float step;
    int tik;
    int type;
} chart_conf_t;

//---------------------------------------------------------------------------

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Chart: public QChart
{
    Q_OBJECT
public:
    Chart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0, int tp = 0);
    virtual ~Chart();
    void setConf(const chart_conf_t *);


public slots:
    void slot_newData(float, void *);
    void slot_newTitle(const QString, void *);

signals:
    void sig_newData(float, void *);
    void sig_newTitle(const QString, void *);

private:   
    //void closeEvent(QCloseEvent *bar) { bar->accept(); }

    QSplineSeries *cser;
    QValueAxis *axX = nullptr, *axY = nullptr;
    qreal step = 2;
    qreal cor_x = 5;
    qreal cor_y = 1;
    min_max_t rx;
    min_max_t ry;
    int tik;
    QVector<QString> titStr;
    int type;
    QStringList sufix;

};

//---------------------------------------------------------------------------

#endif // GRAF_H
