/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/


#ifndef VERSION_H
#define VERSION_H

#include <QString>

const QString vers = QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD);

#endif // VERSION_H
