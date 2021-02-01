/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/



#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "itDialog.h"
#include "graf.h"
#include "compDialog.h"
#include "mainwindow.h"

//******************************************************************************************************

qint32 serSpeed = 115200;


//******************************************************************************************************
//     Реализация конструктора основного класса программы
//
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)//, conf(new SettingsDialog)
{
    ui->setupUi(this);

    logflag = false;
    MyError = 0;


//    this->setWindowOpacity(0.92);//set the level of transparency
    this->setWindowIcon(QIcon(main_pic));
    this->setStyleSheet(tTip);
    ui->log->setReadOnly(true);
    ui->date_time->setStyleSheet("QLineEdit{border: 2px 2px 0px 2px;\
                                            border-color: rgb(0, 0, 0);\
                                            background-color: rgb(183, 183, 183);\
                                            color: rgb(0, 0, 0);}");

    first  = true;
    con = false;
    sdevName = "";
    sdev = nullptr;
    ms10 = 0;
    rxData.clear();

    workFolder = QDir::currentPath();

    logDirName.clear();
    logDirName = workFolder + "/" + def_logDirName;
    QDir dr(logDirName);
    if (!dr.exists()) dr.mkdir(logDirName);

    logFileName.clear();


    //-----   for cmds  -------

    fileName.clear();
    chap.clear();

    qdata.clear();

    addr_graf_t ag = {nullptr, nullptr, nullptr};
    for (int var = 0; var < gNone; ++var) mGraf.append(ag);

    //----  Set timer with period to 10 msec ----
    tmr_sec = startTimer(10);// 10 msec.
    if (tmr_sec <= 0) {
        MyError |= 2;//start_timer error
        throw TheError(MyError);
    }

    //--------------------------------------------

    connect(this, SIGNAL(sigWrite(QByteArray &)), this, SLOT(slotWrite(QByteArray &)));

    //Menu
    connect(ui->actionVERSION,    &QAction::triggered,     this, &MainWindow::About);
    connect(ui->actionCONNECT,    &QAction::triggered,     this, &MainWindow::on_connect);
    connect(ui->actionDISCONNECT, &QAction::triggered,     this, &MainWindow::on_disconnect);
    connect(ui->actionCLEAR,      &QAction::triggered,     this, &MainWindow::clrLog);
    connect(ui->actionDeep,       &QAction::triggered,     this, &MainWindow::goToCharts);
    connect(ui->actionHUMI,       &QAction::triggered,     this, &MainWindow::goToCharts);
    connect(ui->actionTemp,       &QAction::triggered,     this, &MainWindow::goToCharts);
    connect(ui->actionVolt,       &QAction::triggered,     this, &MainWindow::goToCharts);
    connect(ui->actionCOMPAS,     &QAction::triggered,     this, &MainWindow::goToCompas);
    connect(ui->actionDOC,        &QAction::triggered,     this, &MainWindow::docShow);
    connect(ui->actionLogs,       &QAction::triggered,     this, &MainWindow::goToLogs);

    connect(ui->azimut,           &QPushButton::clicked,   this, &MainWindow::goToCompas);
    connect(ui->press,            &QPushButton::clicked,   this, &MainWindow::goToCharts);
    connect(ui->temp,             &QPushButton::clicked,   this, &MainWindow::goToCharts);
    connect(ui->temp2,            &QPushButton::clicked,   this, &MainWindow::goToCharts);
    connect(ui->temp3,            &QPushButton::clicked,   this, &MainWindow::goToCharts);
    connect(ui->humi,             &QPushButton::clicked,   this, &MainWindow::goToCharts);

    connect(this, &MainWindow::sig_newLogDir, this, &MainWindow::slot_newLogDir);
    connect(this, &MainWindow::sigConn,       this, &MainWindow::on_connect);
    connect(this, &MainWindow::sigDisc,       this, &MainWindow::on_disconnect);


    ui->actionCONNECT->setEnabled(true);
    ui->actionPORT->setEnabled(true);

    ui->actionDISCONNECT->setEnabled(false);
    ui->actionCLEAR->setEnabled(false);
    ui->actionDeep->setEnabled(false);
    ui->actionHUMI->setEnabled(false);
    ui->actionCOMPAS->setEnabled(false);
    ui->actionTemp->setEnabled(false);
    ui->actionVolt->setEnabled(false);


    ui->status->clear();

    //---   for speed button   ---
    keyId = 0;
    keys = nullptr;
    keyAdr[keyId++] = ui->stop;
    keyAdr[keyId++] = ui->start;
    keyAdr[keyId++] = ui->bin;
    keyAdr[keyId++] = ui->txt;
    keyAdr[keyId++] = ui->one;
    keyAdr[keyId]   = ui->defmode;
    for (int i = 0; i < keyCnt; ++i) {
        keyArr[i].clear();
        keyArr[i].append(keyData[i]);

        QString tp(keyDataInfo[i]);
        if (tp.indexOf(cr_lf, 0) != -1) tp.truncate(tp.length() - 2);
        keyAdr[i]->setToolTip(tp);
        keyAdr[i]->setText(keyName[i]);

        connect(keyAdr[i], &QPushButton::clicked, this, &MainWindow::keyPrs);
    }
    //-----------------------------

    ui->stx->setText("get");
    ui->stx->setEnabled(false);
    ui->crlfBox->setCheckState(Qt::Checked);
    ui->crlfBox->setEnabled(false);
    ui->answer->setEnabled(false);
    ui->log->setEnabled(false);


    connect(this, &MainWindow::sigButtonData,      this, &MainWindow::slotButtonData);
    connect(this, &MainWindow::sig_checkDataQue,   this, &MainWindow::slot_checkDataQue);
    connect(this, &MainWindow::sig_answer_clicked, this, &MainWindow::on_answer_clicked);


    ui->date_time->setReadOnly(true);

    QFont font = QFont("Helvetica", 10);
    this->setFont(font);

    QString attr = "{border: 4px 4px 4px 4px;\
            border-color: rgb(0, 0, 0);\
            background-color: rgb(183, 183, 183);\
            color: rgb(255, 255, 255);}";
    ui->crlfBox->setStyleSheet("QCheckBox" + attr);


    logHintShow();

    this->setWindowTitle(title);

    //
    conf = new SettingsDialog;
    if (!conf) {
        MyError |= 4;//create settings object error
        throw TheError(MyError);
    }
    //

    goToLogs();

    //
    connect(ui->actionUDP, &QAction::triggered, this, &MainWindow::beginUdp);
    udpSock = nullptr;
    fromDev = new QHostAddress();
    udpPort = 8004;
    //
    srvAddr.clear();
    connect(ui->actionTCP, &QAction::triggered, this, &MainWindow::beginTcp);
    tcpSock = nullptr;
    tcpPort = 8008;
    tmr_tcp = 0;

}
//----------------------------------------------------------------------------------------
//   Реализация диструктор основного класса программы
//
MainWindow::~MainWindow()
{
    deinitSerial();

    if (fromDev) delete fromDev;
    slot_tcpDone();

    if (conf) serSpeed = conf->settings().baudRate;

    if (conf) delete conf;
    if (keys) delete keys;

    chartKill(gNone);
    compKill();

    if (logFile.isOpen()) logFile.close();

    killTimer(tmr_sec);


    delete ui;
}
//-----------------------------------------------------------------------
//-------------------------   UDP   -------------------------------------
//-----------------------------------------------------------------------
void MainWindow::beginUdp()
{
    if (udpSock) {
        if (udpSock->isOpen()) udpSock->close();
        delete udpSock;
        udpSock = nullptr;
        toStatusLine("", picDis);
        if (!tcpSock) {
            ui->log->setEnabled(false);
            ui->actionCLEAR->setEnabled(false);
        }
        return;
    }
    udpSock = new QUdpSocket();
    if (udpSock) {
        if (udpSock->bind(QHostAddress::AnyIPv4, udpPort)) {//, QAbstractSocket::ReuseAddressHint);
            connect(udpSock, SIGNAL(readyRead()), this, SLOT(getUdpPack()));
            udpPack.clear();
            toStatusLine("Wait udp broadcast packets on port " + QString::number(udpPort, 10), picCon);
            ui->log->setEnabled(true);
            ui->actionCLEAR->setEnabled(true);
        }
    }
}
//-----------------------------------------------------------------------
void MainWindow::getUdpPack()
{   
    while (udpSock->hasPendingDatagrams()) {
        udpPack.resize(int(udpSock->pendingDatagramSize()));
        quint64 len = udpSock->readDatagram(udpPack.data(), udpPack.size(), fromDev, &udpPort);
        if (len > 0) {
            QString st(udpPack.constData());
            //
            if (st.indexOf("\r\n") != -1) st.truncate(st.length() - 2);
            if (udpPort == 8004) {
                if (st.indexOf("W5500") != -1) {
                    if (!srvAddr.length()) {
                        srvAddr = fromDev->toString();
                        toStatusLine("Device addr is " + srvAddr, picInfo);
                    }
                }
            }
            //
            LogSave(st);
            udpPack.clear();
        }
    }
}
//-----------------------------------------------------------------------
//-------------------------   TCP   -------------------------------------
//-----------------------------------------------------------------------
void MainWindow::slot_tcpDone()
{
        if (tmr_tcp > 0) {
            killTimer(tmr_tcp);
            tmr_tcp = 0;
        }

        if (tcpSock) {
            tcpSock->disconnect();
            if (tcpSock->isOpen()) tcpSock->close();
            delete tcpSock;
            tcpSock = nullptr;
        }

        if (tcpOut != tcpTimeOut) toStatusLine("", picDis);
        if (!udpSock) {
            ui->log->setEnabled(false);
            ui->actionCLEAR->setEnabled(false);
        }
        ui->err->clear();
        ui->actionTCP->setChecked(false);
        tcpOut = tcpOk;
        tcp_connect = false;

        //ui->actionCLEAR->setEnabled(false);
        ui->actionDeep->setEnabled(false);
        ui->actionHUMI->setEnabled(false);
        ui->actionCOMPAS->setEnabled(false);
        ui->actionTemp->setEnabled(false);
        ui->actionVolt->setEnabled(false);

        ui->stx->setEnabled(false);
        ui->crlfBox->setEnabled(false);
        ui->answer->setEnabled(false);
        //ui->log->setEnabled(false);

        ui->sensor1->setText("Sensor ");
        ui->sensor2->setText("Sensor ");
        ui->sensor3->setText("Sensor ");

}
//-----------------------------------------------------------------------
void MainWindow::beginTcp()
{
    if (tcpSock) {
        slot_tcpDone();
        return;
    }

    if (!srvAddr.length()) {
        QPixmap pm(stop_pic);
        ui->err->setPixmap(pm);
        toStatusLine("Error: No valid server address !", picErr);
        QMessageBox box;
        box.setStyleSheet("color: rgb(255, 255, 255); background-color: rgba(45, 45, 45, 255)");
        box.setIconPixmap(pm);
        box.setText(tr("\nNo valid server address !\n"));
        box.setWindowTitle(tr("Tcp socket Error"));
        box.exec();

        slot_tcpDone();

        return;
    }

    tcpSock = new QTcpSocket();
    if (tcpSock) {
        tcpOut = tcpOk;
        tcpPack.clear();
        ui->log->setEnabled(true);
        ui->actionCLEAR->setEnabled(true);
        toStatusLine("Wait connection to srv " + srvAddr + " until " + QString::number(tmr_tcp_wait/1000, 10) + " sec. ...", picInfo);
        connect(this, &MainWindow::sig_tcpDone, this, &MainWindow::slot_tcpDone);
        connect(this, &MainWindow::sig_tcpTimeOut, this, &MainWindow::slot_tcpTimeOut);
        connect(tcpSock, &QAbstractSocket::connected, this, &MainWindow::slot_tcpConnected);

        //
        tmr_tcp = startTimer(tmr_tcp_wait);// wait connection 5 sec.
        if (tmr_tcp <= 0) {
            MyError |= 2;//start_timer error
            throw TheError(MyError);
        }
        //
        tcpSock->connectToHost(srvAddr, tcpPort, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
        //
    }
}
//-----------------------------------------------------------------------
void MainWindow::slot_tcpConnected()
{
    killTimer(tmr_tcp);
    tmr_tcp = 0;

    connect(tcpSock, SIGNAL(readyRead()), this, SLOT(getTcpPack()));
    connect(tcpSock, &QAbstractSocket::disconnected, this, &MainWindow::slot_tcpDone);

    ui->actionCLEAR->setEnabled(true);
    ui->actionDeep->setEnabled(true);
    ui->actionHUMI->setEnabled(true);
    ui->actionCOMPAS->setEnabled(true);
    ui->actionTemp->setEnabled(true);
    ui->actionVolt->setEnabled(true);

    ui->stx->setEnabled(false);
    ui->crlfBox->setEnabled(false);
    ui->answer->setEnabled(false);
    ui->stx->setEnabled(true);
    ui->crlfBox->setEnabled(true);
    ui->answer->setEnabled(true);
    ui->log->setEnabled(true);

    for (int i = 0; i < keyCnt; ++i) keyAdr[i]->setEnabled(true);

    ui->err->setPixmap(QPixmap(connect_pic));
    toStatusLine("Connection to srv " + srvAddr + " Ok", picCon);

    tcp_connect = true;

}
//-----------------------------------------------------------------------
void MainWindow::slot_tcpTimeOut()
{
    killTimer(tmr_tcp);
    tmr_tcp = 0;
    tcpOut = tcpTimeOut;
    disconnect(this, &MainWindow::sig_tcpTimeOut, this, &MainWindow::slot_tcpTimeOut);
    QPixmap pm(war_pic);
    ui->err->setPixmap(pm);
    toStatusLine("Error: Timeout connection to " + srvAddr, picWar);
    QMessageBox box;
    box.setStyleSheet("color: rgb(255, 255, 255); background-color: rgba(65, 65, 65, 255)");
    box.setIconPixmap(pm);
    box.setText(tr("\nError wait connection : Timeout\n"));
    box.setWindowTitle(tr("Tcp socket Error"));
    box.exec();

    emit sig_tcpDone();

}
//-----------------------------------------------------------------------
void MainWindow::getTcpPack()
{

    tcpPack += tcpSock->readAll();

    int ix = chkDone(&tcpPack);
    if (ix != -1) {
        QString line;
        int pos = 0;
        line.append(tcpPack.mid(0, ix));
        QString lin(line);
        if (lin.indexOf(jBegin, 0) == 0) {
            jsMode = true;
            if ((pos = line.indexOf(jEnd, 0)) != -1) line.remove(pos, jEnd.length());
        } else {
            if ((pos = line.indexOf(cr_lf, 0)) != -1) line.remove(pos, cr_lf.length());
            jsMode = false;
        }

        LogSave(line);
        if (tcpPack.length() >= (ix + 1)) tcpPack.remove(0, ix);
                                     else tcpPack.clear();
        //
        if (line.length() >= 50) {
            if (!jsMode) mkDataFromStr(line);
                    else mkDataFromJson(line);
        }
        //
    }

}
//-----------------------------------------------------------------------
void MainWindow::docShow()
{
    QString fname = "";// qApp->applicationDirPath().append("/" + pdfFileName);
    //if (!QDesktopServices::openUrl(QUrl::fromLocalFile(fname))) {
        QMessageBox box;
        box.setStyleSheet("background-color: rgb(255, 255, 255);");
        box.setIconPixmap(QPixmap(pdf_pic));
        box.setText(tr("Не найден файл документации\n") + fname);
        box.setWindowTitle(tr("Ошибка открытия файла"));
        box.exec();
    //}

}
//-----------------------------------------------------------------------
void MainWindow::slot_newLogDir()
{
    logMutex.lock();
        if (logFile.isOpen()) logFile.close();
        logflag = false;
    logMutex.unlock();

    goToLogs();//open new file
}
//-----------------------------------------------------------------------
void MainWindow::goToLogs()
{
bool lg = logflag;

    logMutex.lock();
    if (!lg) {//запись логов выключена - надо включить (открыть файл)
        if (logFile.isOpen()) {
            logFile.close();
        } else {
            logFileName = logDirName + "/cap_" + QDateTime::currentDateTime().toString("ddMMyyyy_hhmmss") + ".txt";
            logFile.setFileName(logFileName);
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                  lg = true;
                  ui->actionLogs->setChecked(true);
            }
        }
    } else {
        if (logFile.isOpen()) logFile.close();
        lg = false;
    }
    logMutex.unlock();

    logflag = lg;

    logHintShow();
}
//-----------------------------------------------------------------------
void MainWindow::logHintShow()
{
    if (logflag) {
        ui->actionLogs->setToolTip(tr("Протоколирование включено"));
    } else {
        ui->actionLogs->setToolTip(tr("Протоколирование выключено"));
    }
}
//-----------------------------------------------------------------------
void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        //if (lang < all_lang.count()) toStatusLine(tr("Установлен язык интерфейса ") + all_lang.at(lang), picInfo);
   } else {
        QMainWindow::changeEvent(e);
   }
}
//-----------------------------------------------------------------------
//   Обработчик события от таймера - "истекло 10 мсек"
//
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (tmr_sec == event->timerId()) {
        if (!(++ms10 % 100)) ui->date_time->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));

        emit sig_checkDataQue();// отправить сигнал - "проверить есть ли данные в очереди структур"
    } else if (tmr_tcp == event->timerId()) {
        emit sig_tcpTimeOut();
    }
}
//-------------------------------------------------------------------------------------
//   Обработчик события - "нажата клавиша Enter на клавиатуре"
//
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) emit sig_answer_clicked();
}
//-----------------------------------------------------------------------
void MainWindow::keyPrs()
{
    for (int i = 0; i < keyCnt; i++) {
        if (sender() == keyAdr[i]) {
            keyId = i;
            if (sdev || tcp_connect) {
                QByteArray m; m.append(keyArr[keyId]);
                emit sigWrite(m);
            } else {
                emit sigButtonData();
            }
            break;
        }
    }
}
//----------------------------------------------------------------------------------------
void MainWindow::KeyProg(QString str)
{
    if (keyId < keyCnt) {
        if (str.length()) {
            if (keyAdr[keyId]->text() != str) {
                keyArr[keyId].clear();
                keyArr[keyId].append(str + cr_lf);
                keyAdr[keyId]->setText("'" + str + "'");
                keyAdr[keyId]->setToolTip(str);
            }
        }
    }
    if (keys) {
        delete keys;
        keys = nullptr;
    }
}
//--------------------------------------------------------------------------------
//   Метод создает диалоговое окно для программирования кнопки
//
void MainWindow::slotButtonData()
{

    if (keyId < keyCnt) {
        QString from(keyName[keyId] + " key programming");
        QString tp = keyArr[keyId];
        if (tp.indexOf(cr_lf, 0) != -1) tp.truncate(tp.length() - 2);

        if (keys) {
            delete keys;
            keys = nullptr;
        }
        keys = new pwdDialog(nullptr, from, tp);
        if (keys) {
            connect(keys, SIGNAL(DoneW(QString)), this, SLOT(KeyProg(QString)));
            keys->show();
        }
    }
}
//--------------------------------------------------------------------------------
//         Error class
//
MainWindow::TheError::TheError(int err)//error class descriptor
{
    code = err;
}
//
//--------------------------------------------------------------------------------
void MainWindow::clrLog()
{
    ui->log->clear();
}
//--------------------------------------------------------------------------------
int MainWindow::initSerial()
{
    deinitSerial();

    sdev = new QSerialPort(sdevName);
    if (sdev) {
        SettingsDialog::Settings p = conf->settings();
        sdevName = p.name;   sdev->setPortName(sdevName);
        sdev->setBaudRate(p.baudRate);
        sdev->setDataBits(p.dataBits);
        sdev->setParity(p.parity);
        sdev->setFlowControl(p.flowControl);
        sdev->setStopBits(p.stopBits);


        if (!sdev->open(QIODevice::ReadWrite)) {
            delete sdev;
            sdev = nullptr;
            return 1;
        } else {
            // clear incomming buffer
            while (!sdev->atEnd()) rxData = sdev->readAll();
            rxData.clear();
            //
            connect(sdev, &QSerialPort::readyRead, this, &MainWindow::ReadData);
            connect(sdev, &QSerialPort::errorOccurred, this, &MainWindow::slotError);

            return 0;
        }
    } else {
        MyError |= 1;//create serial_port_object error
        throw TheError(MyError);
    }

}
//--------------------------------------------------------------------------------
//   Метод удаляет созданный для последовательного порта объект
//
void MainWindow::deinitSerial()
{
    if (sdev) {
        if (sdev->isOpen()) sdev->close();
        sdev->disconnect();
        delete sdev;
        sdev = nullptr;
        rxData.clear();
    }
}
//--------------------------------------------------------------------------------
//   Без коментариев ....
//
void MainWindow::About()
{

    QString st = QString(tr("\nСервисная программа для устройства\nBlackPill STM32F411 + sensors + LAN"));
    if (sensor1.length() && sensor2.length()) st.append(" :\n" + sensor1 + ", " + sensor2 + ", " + sensor3);
    st.append("\nВерсия " + qApp->applicationVersion() + tr(" ") + BUILD);

    QMessageBox box;
    box.setStyleSheet("background-color: rgb(208, 208, 208);");
    box.setIconPixmap(QPixmap(salara_pic));
    box.setText(st);
    box.setWindowTitle(tr("О программе"));

    box.exec();

}
//-----------------------------------------------------------------------
//   Метод выводит графический элемент и текстовую строку в поле "статус"
//
void MainWindow::toStatusLine(QString st, int pic)
{
    ui->status->clear();

    if (!st.length()) {
        ui->pic->clear();
        return;
    }

    QString pix;

    switch (pic) {
        case picClr:
            ui->pic->clear();
        break;
        case picInfo:
            pix = info_pic;
        break;
        case picTime:
            pix = timeout_pic;
        break;
        case picWar:
            pix = war_pic;
        break;
        case picErr:
            pix = err_pic;
        break;
        case picCon:
            pix = ucon_pic;
        break;
        case picDis:
            pix = udis_pic;
        break;
        case picOk:
            pix = ok_pic;
        break;
    }

    ui->pic->setPixmap(QPixmap(pix));
    ui->status->setText(st);
}
//-----------------------------------------------------------------------
//   Метод выводит в окне текстовые данные (логи) обмена с устройством
//
void MainWindow::LogSave(const QString & st)
{
    if (dbg) {
        ui->log->append(st);
        if (logflag) {
            if (st.length()) {
                QByteArray data;
                data.append(st);
                if (data.at(data.length() - 1) != 0x0a) data.append(0x0a);
                logMutex.lock();
                    logFile.write(data);
                    logFile.flush();
                logMutex.unlock();
            }
        }
    }
}
//-----------------------------------------------------------------------
//   Обработчик события - "подключиться к последовательному порту"
//
void MainWindow::on_connect()
{
    if (con) return;


    if (!initSerial()) {
        toStatusLine(tr("Подключен к %1 : %2 %3%4%5 FlowControl %6")
                          .arg(sdevName)
                          .arg(conf->settings().stringBaudRate)
                          .arg(conf->settings().stringDataBits)
                          .arg(conf->settings().stringParity.at(0))
                          .arg(conf->settings().stringStopBits)
                          .arg(conf->settings().stringFlowControl), picCon);
        con = true;
        ui->actionCONNECT->setEnabled(false);
        ui->actionPORT->setEnabled(false);

        ui->actionDISCONNECT->setEnabled(true);
        ui->actionCLEAR->setEnabled(true);
        ui->actionDeep->setEnabled(true);
        ui->actionHUMI->setEnabled(true);
        ui->actionCOMPAS->setEnabled(true);
        ui->actionTemp->setEnabled(true);
        ui->actionVolt->setEnabled(true);

        ui->stx->setEnabled(false);
        ui->crlfBox->setEnabled(false);
        ui->answer->setEnabled(false);
        ui->stx->setEnabled(true);
        ui->crlfBox->setEnabled(true);
        ui->answer->setEnabled(true);
        ui->log->setEnabled(true);

        for (int i = 0; i < keyCnt; ++i) keyAdr[i]->setEnabled(true);

        ui->actionPORT->setToolTip(tr("Параметры последовательного порта:\n%1 %2 %3%4%5")
                                   .arg(sdevName)
                                   .arg(conf->settings().stringBaudRate)
                                   .arg(conf->settings().stringDataBits)
                                   .arg(conf->settings().stringParity.at(0))
                                   .arg(conf->settings().stringStopBits));

    } else {
        toStatusLine(tr("Ошибка при открытии последовательного порта ") + sdevName, picErr);
        deinitSerial();
    }
}
//-----------------------------------------------------------------------
//   Обработчик события - "отключиться от последовательного порта"
//
void MainWindow::on_disconnect()
{

    //chartKill(gNone);

    if (!con) return;

    con = false;

    deinitSerial();

    toStatusLine(tr("Отключен от последовательного порта ") + sdevName, picDis);

    ui->actionPORT->setEnabled(true);
    ui->actionCONNECT->setEnabled(true);

    ui->actionDISCONNECT->setEnabled(false);
    ui->actionCLEAR->setEnabled(false);
    ui->actionDeep->setEnabled(false);
    ui->actionHUMI->setEnabled(false);
    ui->actionCOMPAS->setEnabled(false);
    ui->actionTemp->setEnabled(false);
    ui->actionVolt->setEnabled(false);

    ui->stx->setEnabled(false);
    ui->crlfBox->setEnabled(false);
    ui->answer->setEnabled(false);
    ui->log->setEnabled(false);

    ui->sensor1->setText("Sensor ");
    ui->sensor2->setText("Sensor ");
    ui->sensor3->setText("Sensor ");

}
//-----------------------------------------------------------------------
//   Метод-слот осуществляет выдачу стоки данных на устройство
//    через последовательный интерфейс
//
void MainWindow::slotWrite(QByteArray & mas)
{
    toStatusLine("", picClr);

    if (sdev || tcpSock) {
        QString m(mas);
        if (m.indexOf("epoch") != -1) {
            m.append("=" + QString::number((int)(QDateTime::currentDateTime().toTime_t()), 10) + ":2\r\n");
        }

        if (sdev) {
            if (sdev->isOpen()) sdev->write(m.toLocal8Bit());
        }
        if (tcpSock) {
            if (tcp_connect) tcpSock->write(m.toLocal8Bit());
        }
    }
}
//-----------------------------------------------------------------------
void MainWindow::on_answer_clicked()
{
    QByteArray tmp = ui->stx->text().toLocal8Bit();

    if (ui->crlfBox->checkState() == Qt::Checked) tmp.append(cr_lf);

    if (tmp.length() >= 2) emit sigWrite(tmp);
}
//------------------------------------------------------------------------------------
//  HEX string to BIN byte function
//
unsigned char MainWindow::myhextobin(const char *uk)
{
char line[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
unsigned char a = 0, b = 0, c = 0, i;

    for	(i = 0; i < 16; i++) { if (*uk == line[i]) { b = i; break; } else b = 0xff; }
    for	(i = 0; i < 16; i++) { if (*(uk + 1) == line[i]) { c = i; break; } else c = 0xff; }
    if ((b == 0xff) || (c == 0xff)) a = 0xff; else { b <<= 4;   a = b | c; }

    return a;
}
//-----------------------------------------------------------------------
//  Функция преобразует hex-строку в бинарный массив
//
void MainWindow::hexTobin(const char *in, QByteArray *out)
{
size_t i = 0, len = strlen(in) / 2;
const char *uk = in;

    while (i < len) {
        out->append( static_cast<char>(myhextobin(uk)) );
        i++; uk += 2;
    }
}
//-----------------------------------------------------------------------
//   Метод определения события "ответ получен" при приеме данных от устройства
//
int MainWindow::chkDone(QByteArray *buf)
{
int ret = -1;        

    if (buf->indexOf(jBegin, 0) == 0) {
        ret = buf->indexOf(jEnd, 0);
        if (ret != -1) ret += jEnd.length();
    } else {
        ret = buf->indexOf(cr_lf, 0);
        if (ret != -1) ret += cr_lf.length();
    }

    return ret;
}
//-----------------------------------------------------------------------
void MainWindow::ReadData()
{
int ix = -1;

    while (!sdev->atEnd()) {
        rxData += sdev->readAll();
        ix = chkDone(&rxData);
        if (ix != -1) {
            QString line;
            int pos = 0;
            line.append(rxData.mid(0, ix));
            QString lin(line);
            if (lin.indexOf(jBegin, 0) == 0) {
                jsMode = true;
                if ((pos = line.indexOf(jEnd, 0)) != -1) line.remove(pos, jEnd.length());
            } else {
                if ((pos = line.indexOf(cr_lf, 0)) != -1) line.remove(pos, cr_lf.length());
                jsMode = false;
            }

            LogSave(line);
            if (rxData.length() >= (ix + 1)) rxData.remove(0, ix);
                                        else rxData.clear();
            //
            if (line.length() >= 50) {
                if (!jsMode) mkDataFromStr(line);
                        else mkDataFromJson(line);
            }
            //
            break;
        }
    }

}
//-----------------------------------------------------------------------
void MainWindow::slotError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        MyError |= 8;//error reading from serial port
        throw TheError(MyError);
    }
}
//-----------------------------------------------------------------------------------
void MainWindow::slot_checkDataQue()
{
    if (qdata.isEmpty()) return;

    qdata_t one = qdata.dequeue();


    //-------   send data to chart   -------
    int i = -1, go = 1;
    float val;
    while (++i < gNone) {
        if (mGraf[i].graf != nullptr) {
            switch (i) {
                case gVolt : val = one.volt;   go = 1; break;
                case gDeep : val = one.pres;   go = 1; break;
                case gTemp : val = one.temp;   go = 1; break;
                case gTemp2: val = one.temp2;  go = 1; break;
                case gTemp3: val = one.temp3;  go = 1; break;
                case gHumi : val = one.humi;   go = 1; break;
                //case gComp : val = one.azimut; go = 1; break;
                    default : go = 0;
            }
            if (go) emit sig_toChart(val , mGraf[i].pic);
        }
    }
    //--------------------------------------

    if (compas) emit sig_updateValue(one.azimut);//, one.xyz);

    QString dt;
    dt.sprintf("FIFO=%u/%u ERR=%02X VOLT=%.3fv PRES=%.2fmmHg TEMP=%.2fC HUMI=%.2f%% AZIMUT=%.2fC TEMP2=%.2fC TEMP3=%.2fC ACCEL=%d,%d,%d GYRO=%d,%d,%d",
                one.fifo.cur, one.fifo.max,
                one.err,
                one.volt,
                one.pres,
                one.temp,
                one.humi,
                one.azimut,
                one.temp2,
                one.temp3,
                one.accel[0], one.accel[1], one.accel[2],
                one.gyro[0], one.gyro[1], one.gyro[2]);

    if (dt.length()) {
        int pic = picInfo;
        if (one.err) pic = picErr;
        toStatusLine(dt, pic);


        const QString *ps =  &devOk_pic;
        QString ttip("No error");
        if (one.err) {
            ps =  &devErr_pic;
            ttip.sprintf("Error : 0x%X", one.err);
        }
        ui->err->setPixmap(QPixmap(*ps));
        //ui->err->setText(QString::number(one.err, 16));
        ui->err->setToolTip("No error");

        ui->sensor1->setText(sensor1);
        ui->sensor2->setText(sensor2);
        ui->sensor3->setText(sensor3);
        dt.sprintf("Давление\n%.2f mmHg",  one.pres);  ui->press->setText(dt);
        dt.sprintf("Температура\n%.2f C",  one.temp);  ui->temp->setText(dt);
        dt.sprintf("Влажность\n%.2f %%",   one.humi);  ui->humi->setText(dt);
        dt.sprintf("Азимут\n%.2f deg.",  one.azimut);  ui->azimut->setText(dt);
        dt.sprintf("Температура\n%.2f C", one.temp2);  ui->temp2->setText(dt);
        dt.sprintf("Температура\n%.2f C", one.temp3);  ui->temp3->setText(dt);
        dt.sprintf("Акселерометр\n%d , %d , %d", one.accel[0], one.accel[1], one.accel[2]);  ui->accel->setText(dt);
        dt.sprintf("Гироскоп\n%d , %d , %d", one.gyro[0], one.gyro[1], one.gyro[2]);  ui->gyro->setText(dt);
    }
}
//-----------------------------------------------------------------------------------
void MainWindow::mkDataFromStr(QString str)
{

    if (str.indexOf(" fifo:", 0) == -1) return;

    int pos = 0, begin = 0, end = str.length() - 1;
    int cur = begin, val = -1, pend = 0;
    float fval = 0.0;
    QString tmp;
    qdata_t one;
    bool ok = false;

    memset((uint8_t *)&one, 0, sizeof(qdata_t));

    pos = str.indexOf(" BM", 0);
    if (pos != -1) {
        pos++;
        pend = str.indexOf(":", pos);
        if (pend == -1) str.indexOf(" ", pos);
        if (pend != -1) sensor1 = str.mid(pos, pend - pos);
    }
    pos = str.indexOf(" QMC", 0);
    if (pos != -1) {
        pos++;
        pend = str.indexOf(":", pos);
        if (pend == -1) str.indexOf(" ", pos);
        if (pend != -1) sensor2 = str.mid(pos, pend - pos);
    }
    pos = str.indexOf(" MPU", 0);
    if (pos != -1) {
        pos++;
        pend = str.indexOf(":", pos);
        if (pend == -1) str.indexOf(" ", pos);
        if (pend != -1) sensor3 = str.mid(pos, pend - pos);
    }



    for (int i = 0; i < parCnt; i++) {
        pos = str.indexOf(parName[i], cur);
        if (pos != -1) {
            tmp.clear();
            begin = pos + parName[i].length();
            end = str.indexOf(" ", begin + 1);
            if (end == -1) end = str.length();// - 1;
            switch (i) {
                case qFifo :
                    one.fifo.cur = one.fifo.max = -1;
                    if (end != -1) {
                        tmp = str.mid(begin, end - begin);
                        if (tmp.length()) {
                            pos = tmp.indexOf("/", 0);
                            if (pos != -1) {
                                QString tp = tmp.mid(pos + 1);
                                if (tp.length()) {
                                    val = tp.toInt(&ok, 10);
                                    if (ok) {
                                        one.fifo.max = val;
                                        tp = tmp.mid(0, pos);
                                        if (tmp.length()) {
                                            val = tp.toInt(&ok, 10);
                                            if (ok) one.fifo.cur = val;
                                        }
                                    }
                                }
                            }
                        }
                    }
                break;
                case qErr :
                    if (end != -1) {
                        if (end > begin) {
                            tmp = str.mid(begin, end - begin);
                            val = tmp.toUInt(&ok, 16);
                            if (ok) one.err = val;
                        }
                    }
                break;
                case qVolt :
                    if (end != -1) {
                        if (end > begin) {
                            tmp = str.mid(begin, end - begin);
                            fval = tmp.toFloat(&ok);
                            if (ok) one.volt = fval;
                        }
                    }
                break;
                case qDeep :
                    if (end != -1) {
                        if (end > begin) {
                            tmp = str.mid(begin, end - begin);
                            fval = tmp.toFloat(&ok);
                            if (ok) one.pres = fval;
                        }
                    }
                break;
                case qTemp :
                    if (end != -1) {
                        if (end > begin) {
                            tmp = str.mid(begin, end - begin);
                            fval = tmp.toFloat(&ok);
                            if (ok) one.temp = fval;
                        }
                    }
                break;
                case qHumi :
                    if (end != -1) {
                        if (end > begin) {
                            tmp = str.mid(begin, end - begin);
                            fval = tmp.toFloat(&ok);
                            if (ok) one.humi = fval;
                        }
                    }
                break;
                case qComp :
                    if (end != -1) {
                        if (end > begin) {
                            tmp = str.mid(begin, end - begin);
                            fval = tmp.toFloat(&ok);
                            if (ok) one.azimut = fval;
                        }
                    }
                break;
                case qTemp2 :
                    if (end != -1) {
                        if (end > begin) {
                            tmp = str.mid(begin, end - begin);
                            fval = tmp.toFloat(&ok);
                            if (ok) one.temp2 = fval;
                        }
                    }
                break;
                case qTemp3 :
                    if (end != -1) {
                        if (end > begin) {
                            tmp = str.mid(begin, end - begin);
                            fval = tmp.toFloat(&ok);
                            if (ok) one.temp3 = fval;
                        }
                    }
                break;    
                case qAccel :
                case qGyro :
                    if (end != -1) {
                        if (end > begin) {
                            tmp = str.mid(begin, end - begin);
                            int len = MAX_ACC_CNT;
                            if (i != qAccel) len = MAX_ACC_CNT;
                            QStringList ls = tmp.split(",");
                            if (ls.count() == len) {
                                short cr = 0;
                                for (int j = 0; j < len; j++) {
                                    if (ls.at(j).length()) {
                                        cr = ls.at(j).toShort(&ok, 10);
                                        if (ok) {
                                            if (i == qAccel) {
                                                one.accel[j] = cr;
                                            } else {
                                                one.gyro[j] = cr;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                break;
            }
        }
    }

    last_vals = one;
    qdata.enqueue(one);

}
//----------------------------------------------------------------------------------------------------------------------------
void MainWindow::mkDataFromJson(QString str)
{
int i, j, val;
float fval;
QJsonParseError err;
QByteArray buf;



    buf.append(str);
    QJsonDocument jdoc= QJsonDocument::fromJson(buf , &err);
    if (err.error != QJsonParseError::NoError) {
        QString stx = "Command parser error : " + err.errorString() + "\n";
        toStatusLine(stx, picErr);
        return;
    }
    QJsonObject *obj = new QJsonObject(jdoc.object());
    if (!obj) {
        QString stx = "Command parser error : " + err.errorString() + "\n";
        toStatusLine(stx, picErr);
        return;
    }

/*{"time":"11.01 14:58:46",
    "ms":571010,
    "fifo":[1,3],
    "devError":0,
    "volt":3.290,
    "BME280":{"press":757.34,"temp":26.19,"humi":27.42},
    "QMC5883L":{"azimut":300.45,"temp2":27.8},
    "MPU6050":{"stat":1,"temp3":26.99,"accel":[0,0,0],"gyro":[0,0,-1]}}*/

    qdata_t one;
    QJsonValue tmp;
    QJsonObject *tobj = nullptr;

    for (i = 0; i < jnameCnt; i++) {
        tmp = obj->value(jsonAllName[i]);
        if (!tmp.isUndefined()) {
            if (tmp.isArray()) {
                int ct = -1, alen = tmp.toArray().size();
                while (++ct < alen) {
                    if (i == jfifo) {
                        val = tmp.toArray().at(ct).toInt();
                        if (ct == 0) one.fifo.cur = val;
                        else
                        if (ct == 1) one.fifo.max = val;
                    }
                }
            } else if (tmp.isDouble()) {
                fval = tmp.toDouble();
                if (i == jvolt) one.volt = fval;
            } else if (tmp.isObject()) {
                tobj = new QJsonObject(tmp.toObject());
                if (i == jbmx) {
                    sensor1 = jsonAllName[i];
                    for (j = 0; j < js1Cnt; j++) {
                        tmp = tobj->value(js1Name[j]);
                        fval = tmp.toDouble();
                        switch (j) {
                            case jpress: one.pres = fval; break;
                            case jtemp: one.temp = fval; break;
                            case jhumi: one.humi = fval; break;
                        }
                    }
                } else if (i == jqmc) {
                    sensor2 = jsonAllName[i];
                    for (j = 0; j < js2Cnt; j++) {
                        tmp = tobj->value(js2Name[j]);
                        fval = tmp.toDouble();
                        switch (j) {
                            case jazimut: one.azimut = fval; break;
                            case jtemp2:  one.temp2  = fval; break;
                        }
                    }
                } else if (i == jmpu) {
                    sensor3 = jsonAllName[i];
                    for (j = 0; j < js3Cnt; j++) {
                        tmp = tobj->value(js3Name[j]);
                        if (tmp.isDouble()) {
                            fval = tmp.toDouble();
                            if (j == jtemp3) one.temp3 = fval;
                        } else if (tmp.isArray()) {
                            //
                            int ct = -1, alen = tmp.toArray().size();
                            while (++ct < alen) {
                                val = tmp.toArray().at(ct).toInt();
                                if (j == jaccel) one.accel[ct] = val;
                                else
                                if (j == jgyro) one.gyro[ct] = val;
                            }
                            //
                        }
                    }
                }
                if (tobj) {
                    delete tobj;
                    tobj = nullptr;
                }
            } else {
                val = tmp.toInt();
                if (i == jerr) one.err = val;
            }
            //
        }
    }

    if (obj) delete obj;

    last_vals = one;
    qdata.enqueue(one);

}
//----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
uint32_t MainWindow::get10ms()
{
    return ms10;
}
//---------------------------------------
uint32_t MainWindow::get_tmr(uint32_t tm)
{
    return (get10ms() + tm);
}
//---------------------------------------
int MainWindow::check_tmr(uint32_t tm)
{
    return (get10ms() >= tm ? 1 : 0);
}
//----------------------------------------------------------------------
int MainWindow::writes(const char *data, int len)
{
int ret = 0;

    if (sdev) {
        if (sdev->isOpen()) ret = static_cast<int>(sdev->write(data, len));
    }

    return ret;
}
//--------------------------------------------------------------------------------

//*********************************************************************************************
//                            support charts window
//*********************************************************************************************

//----------------------------------------------------------------------
void MainWindow::chartKill(int id)
{
int i, from = 0, to = gNone;

    if (id < gNone) {
        from = id;
        to = ++id;
    }

    for (i = from; i < to; i++) {
        if (mGraf[i].graf != nullptr) {
            if (mGraf[i].cv != nullptr) delete reinterpret_cast<QChartView *>(mGraf[i].cv);
            mGraf[i].cv = nullptr;

            delete reinterpret_cast<QDialog *>(mGraf[i].graf);
            mGraf[i].graf = nullptr;

            mGraf[i].pic = nullptr;
        }
    }
}
//----------------------------------------------------------------------
void MainWindow::slot_chartDone()
{
    for (int i = 0; i < gNone; i++) {
        if (sender() == mGraf[i].graf) {
            chartKill(i);
            return;
        }
    }

}
//--------------------------------------------------------------------------------
void MainWindow::goToCharts()
{
    // проверка есть ли данные для затребованного графика
    //
    idgraf = gNone;
    QString addTitle;

    if ((sender() == ui->actionDeep) || (sender() == ui->press)) {
        idgraf = gDeep;
        addTitle = sensor1;
    } else if ((sender() == ui->actionHUMI) || (sender() == ui->humi)) {
        idgraf = gHumi;
        addTitle = sensor1;
    } else if ((sender() == ui->actionTemp) || (sender() == ui->temp)) {
        idgraf = gTemp;
        addTitle = sensor1;
    } else if (sender() == ui->temp2) {
        idgraf = gTemp2;
        addTitle = sensor2;
    } else if (sender() == ui->temp3) {
        idgraf = gTemp3;
        addTitle = sensor3;
    } else if (sender() == ui->actionVolt) {
        idgraf = gVolt;
        addTitle = "ADC";
    }
    if (idgraf == gNone) return;

    //
    if (mGraf[idgraf].graf) {
        toStatusLine(tr("Этот график уже создан !"), picInfo);
        return;
    }


    graf = new QDialog(this);//nullptr);// создаем диалоговое окно для графика
    if (graf) {
        QIcon ic = QIcon(QPixmap(chart_pic));
        graf->setWindowTitle(title + " | " + addTitle);
        graf->setWindowFlags(Qt::Dialog);
        graf->resize(460, 340);
        QSize sz = graf->size();
        graf->setFixedSize(sz);

        pic = new Chart(nullptr, 0, idgraf);// создаем собственно объект графика
        if (pic) {

            chart_conf_t chart_conf = {
                .name = "???",
                .cx = MAX_AXIS_X / 2,             //start x
                .cy = 0,                          //start y
                {MIN_AXIS_X, MAX_AXIS_X},         //minx, maxx
                {MIN_DEEP_VALUE, MAX_DEEP_VALUE}, //miny, maxy
                1.0,                              //step
                10,                               //tik
                .type = idgraf
            };

            const QString *icu = nullptr;

            switch (idgraf) {
                case gDeep:
                    icu = &deep_pic;
                    chart_conf.cy = last_vals.pres - chart_conf.step;
                    chart_conf.name = tr("Атмосферное давление");
                    chart_conf.ry.min = MIN_DEEP_VALUE;
                    chart_conf.ry.max = MAX_DEEP_VALUE;
                break;
                case gHumi:
                    icu = &humi_pic;
                    chart_conf.cy = last_vals.humi - chart_conf.step;
                    chart_conf.name = tr("Влажность воздуха");
                    chart_conf.ry.min = MIN_HUMI_VALUE;
                    chart_conf.ry.max = MAX_HUMI_VALUE;
                break;
                case gTemp:
                case gTemp2:
                case gTemp3:
                    icu = &temp_pic;
                    if (idgraf == gTemp) {
                        chart_conf.cy = last_vals.temp - chart_conf.step;
                    } else if (idgraf == gTemp2) {
                        chart_conf.cy = last_vals.temp2 - chart_conf.step;
                    } else {
                        chart_conf.cy = last_vals.temp3 - chart_conf.step;
                    }
                    chart_conf.name = tr("Температура воздуха");
                    chart_conf.ry.min = MIN_TEMP_VALUE;//-20
                    chart_conf.ry.max = MAX_TEMP_VALUE;//50;
                break;
                case gVolt:
                    icu = &volt_pic;
                    chart_conf.cy = last_vals.volt - chart_conf.step;
                    chart_conf.name = tr("Напряжение питания");
                    chart_conf.ry.min = MIN_VOLT_VALUE;//6
                    chart_conf.ry.max = MAX_VOLT_VALUE;//40;
                break;
                    default: chart_conf.step = 1.0;
            }
            ic = QIcon(QPixmap(*icu));
            chart_conf.ry.min *= chart_conf.step;
            chart_conf.ry.max *= chart_conf.step;
            pic->setConf(&chart_conf);
            pic->legend()->hide();
            pic->setAnimationOptions(QChart::SeriesAnimations);

            cView = new QChartView(pic);// создаем область прорисовки графика
            if (cView) {
                cView->setRenderHint(QPainter::Antialiasing);
                cView->setParent(graf);
                cView->resize(sz);
                connect(this, &MainWindow::sig_toChart, pic, &Chart::slot_newData);
            } else {
                delete pic;
                pic = nullptr;
                delete graf;
                graf = nullptr;
                return;
            }
        } else {
            delete graf;
            graf = nullptr;
            return;
        }

        graf->setWindowIcon(ic);
        connect(graf, &QDialog::finished, this, &MainWindow::slot_chartDone);

        // запоминаем данные созданного графика (адреса) в векторе
        mGraf[idgraf].cv   = cView;
        mGraf[idgraf].pic  = pic;
        mGraf[idgraf].graf = graf;

        graf->show();

    }
}
//--------------------------------------------------------------------------------

//*********************************************************************************************
//                            support compas window
//*********************************************************************************************

//----------------------------------------------------------------------
void MainWindow::compKill()
{
    if (compas) {
        compas->disconnect();
        delete compas;
        compas = nullptr;
    }
}
//--------------------------------------------------------------------------------
void MainWindow::goToCompas()
{
    compKill();

    compas = new compDialog(this);// создаем диалоговое окно для размещения таблицы
    if (compas) {
        QIcon ic = QIcon(QPixmap(compas_pic));
        compas->setWindowTitle(title + " | " + sensor2);
        //
        connect(this, &MainWindow::sig_updateValue, compas, &compDialog::updateValue);
        //
        compas->show();
    }

}
//--------------------------------------------------------------------------------

//*********************************************************************************************




