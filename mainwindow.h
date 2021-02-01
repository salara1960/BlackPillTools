/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <inttypes.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__
    #include <arpa/inet.h>
    #include <endian.h>
    #include <termios.h>
    #include <unistd.h>
#else
    #include <windows.h>
#endif

#include <QSystemTrayIcon>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDateTime>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QPixmap>
#include <QCheckBox>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFileDialog>
#include <QIODevice>
#include <QComboBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTableWidgetItem>
#include <QTableView>
#include <QHeaderView>
#include <QQueue>
#include <QAbstractItemView>
#include <QtCharts>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include <QGraphicsItem>
#include <QtCharts/QChartView>
#include <QVector>
#include <QMutex>

#include <QUrl>
#include <QtQuickWidgets/QQuickWidget>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QtQml>

#include <QtNetwork>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QtNetwork/QAbstractSocket>
#include <QHostAddress>


#include "defs.h"


//********************************************************************************

//********************************************************************************

const QString title = "BlackPill Util";


const QString con_pic     = "png/conn.png";
const QString dis_pic     = "png/dis.png";
const QString salara_pic  = "png/niichavo.png";
const QString close_pic   = "png/close.png";
const QString disk_pic    = "png/disk.png";
const QString ucon_pic    = "png/uCon.png";
const QString udis_pic    = "png/uDis.png";
const QString timeout_pic = "png/wait.png";
const QString info_pic    = "png/info1.png";
const QString war_pic     = "png/warning.png";
const QString err_pic     = "png/error.png";
const QString ok_pic      = "png/ok.png";
const QString chart_pic   = "png/chart.png";
const QString compas_pic  = "png/compass.png";
const QString deep_pic    = "png/deep.png";
const QString humi_pic    = "png/speed.png";
const QString temp_pic    = "png/temp.png";
const QString volt_pic    = "png/battery.png";
const QString main_pic    = "png/izba.ico";
const QString logon_pic   = "png/log.png";
const QString ini_pic     = "png/settings.png";
const QString pdf_pic     = "png/pdf.png";
const QString stop_pic    = "png/Stop.png";

const QString devErr_pic  = "png/devErr.png";
const QString devOk_pic   = "png/devOk.png";

const QString tcpCon_pic  = "png/conPC.png";
const QString tcpDis_pic  = "png/disconPC.png";
const QString connect_pic  = "png/connect.png";


const QString tTip = "QToolTip { color: blue; background-color: white; border: 2px solid #676767; }";
extern qint32 serSpeed;

//********************************************************************************

namespace Ui {
class MainWindow;
}

class SettingsDialog;
class pwdDialog;
class Chart;
class compDialog;

//********************************************************************************
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    class TheError {
        public :
            int code;
            TheError(int);
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int chkDone(QByteArray *buf);
    void toStatusLine(QString, int);
    //
    QString logDirName;
    //

protected:
    virtual void timerEvent(QTimerEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *) override;
    virtual void changeEvent(QEvent *e) override;


public slots:

    void slotButtonData();
    void KeyProg(QString);
    int initSerial();
    void deinitSerial();
    void LogSave(const QString &);
    void About();
    unsigned char myhextobin(const char *);
    void hexTobin(const char *, QByteArray *);
    void clrLog();
    int writes(const char *data, int len);
    //
    uint32_t get10ms();
    uint32_t get_tmr(uint32_t);
    int check_tmr(uint32_t);
    //
    void logHintShow();
    void slot_newLogDir();

private slots:

    void ReadData();
    void slotError(QSerialPort::SerialPortError);
    void keyPrs();
    void on_answer_clicked();
    void on_connect();
    void on_disconnect();

//    void picUpdateTitle();
    void chartKill(int);
    void slot_chartDone();
    void goToCharts();    
    void goToCompas();
    void compKill();

    void slot_checkDataQue();
    void mkDataFromStr(QString);
    void mkDataFromJson(QString);

    void slotWrite(QByteArray &);

    void goToLogs();
    void docShow();

    void getUdpPack();
    void beginUdp();

    void beginTcp();
    void slot_tcpConnected();
    void slot_tcpTimeOut();
    void slot_tcpDone();
    void getTcpPack();


signals:

    void sigWrite(QByteArray &);
    void sigAbout();
    void sigButtonData();
    void sigConn();
    void sigDisc();
    void sig_answer_clicked();
    void sig_checkDataQue();
    void sig_toChart(float, void *);
    void sig_updateValue(float);//, QList<int>);
    void sig_tik();
    void sig_newLogDir();

    void sig_tcpConnected();
    void sig_tcpTimeOut();
    void sig_tcpDone();


private:
    Ui::MainWindow *ui;
    int tmr_sec, MyError;
    QSerialPort *sdev;
    QByteArray rxData;
    QString sdevName, sdevConf;
    bool first, con, logflag = false;
    QFile logFile;
    QString logFileName;
    QMutex logMutex;

    const QString def_logDirName = "logs/";
    const QString devErrMask = "devError:";

    int keyId;
    QString keyArr[keyCnt];
    pwdDialog *keys;
    QPushButton *keyAdr[keyCnt];
    QString keyName[keyCnt] = {"rst", "udp_off", "udp_on", "epoch", "set min\nperiod", "set max\nperiod"};
    const QString keyDataInfo[keyCnt] = {"Restart", "Shift On/Off", "Set time", "Min period", "Max period", ""};
    const QString keyData[keyCnt] = {"rst\r\n", "udp_off\r\n", "udp_on\r\n", "epoch", "period=1000\r\n", "period=5000\r\n"};
    //
    const QString parName[parCnt] = {"fifo:", "devError=", "Vcc=", "pres=", "temp=", "humi=", "azimut=", "temp2=", "temp3=", "accel=", "gyro="};
    //
    const QString jsonAllName[jnameCnt] = {
        "time",
        "ms",
        "fifo",
        "err",
        "volt",
        "BME280",
        "QMC5883L",
        "MPU6050"
    };
    const QString js1Name[js1Cnt] = {
        "pres",
        "temp",
        "humi",
    };
    const QString js2Name[js2Cnt] = {
        "azimut",
        "temp2"
    };
    const QString js3Name[js3Cnt] = {
        "stat",
        "temp3",
        "accel",
        "gyro"
    };

    bool jsMode = false;


    //
    //settings
    SettingsDialog *conf = nullptr;
    uint32_t ms10;

    uint8_t dbg = 1;

    uint8_t devErr;
    int ackLen, ibuff_len = 0;
    char buff[BUF_SIZE + 8] = {0};
    char ibuff[BUF_SIZE + 8] = {0};
    uint8_t to_dev_data[TO_DEV_SIZE];
    QByteArray fileName;
    QString chap;
    const QByteArray cr_lf = "\r\n";
    const QByteArray jBegin = "{";
    const QByteArray jEnd = ">\r\n";
    const QString formatTime = "dd.MM.yyyy hh:mm:ss.zzz";

    QQueue<qdata_t> qdata;
    qdata_t last_vals;

    QDialog *graf = nullptr;
    QChartView *cView = nullptr;
    Chart *pic = nullptr;
    QLineSeries *series = nullptr;
    int idgraf = gDeep;
    QVector<addr_graf_t> mGraf;

    QString workFolder;
    bool patch = false;

    QString sensor1, sensor2, sensor3;

    compDialog *compas = nullptr;

    QUdpSocket *udpSock = nullptr;
    QByteArray udpPack;
    QHostAddress *fromDev = nullptr;
    quint16 udpPort = 8004;

    QString srvAddr;

    QTcpSocket *tcpSock = nullptr;
    QByteArray tcpPack;
    quint16 tcpPort = 8008;
    int tmr_tcp;
    const int tmr_tcp_wait = 5000;
    uint8_t tcpOut;
    bool tcp_connect = false;


};



#endif // MAINWINDOW_H
