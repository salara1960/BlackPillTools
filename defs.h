/*
    Автор : Ильминский А.Н.
    Дата : 28.12.2020
    Сервисная прогамма для работы с устройством "BlackPillDevice"
*/


#ifndef DEFS_H
#define DEFS_H


#include <inttypes.h>
#include <QString>
#include <QDateTime>

#include "version.h"
#include "bnumber.h"

//***********************************************************************************

#define SET_DEBUG



#define NEXT 0
#define LAST 1

#define MAX_PARAM_NAME 3

#define MAX_CFG_PARAM 7

#define BUF_SIZE          2048
#define max_buf           2048
#define keyCnt            8//6
#define parCnt            11
#define jnameCnt          8
#define js1Cnt            3
#define js2Cnt            2
#define js3Cnt            4

#define TO_DEV_SIZE       1024

#define MIN_DEEP_VALUE    0// mmHg
#define MAX_DEEP_VALUE  800// mmHg
#define MIN_HUMI_VALUE    0// %
#define MAX_HUMI_VALUE  100// %
#define MIN_TEMP_VALUE  -40// град Цельсия
#define MAX_TEMP_VALUE   50// град Цельсия
#define MIN_VOLT_VALUE    0// вольты
#define MAX_VOLT_VALUE    6// вольты
#define MIN_AXIS_X        0
#define MAX_AXIS_X       12

#define MAX_ACC_CNT       3
#define MAX_GYR_CNT       3



#define _1s 100
#define _2s _1s * 2
#define _3s _1s * 3
#define _4s _1s * 4
#define _5s _1s * 5



// Маркосы-аналоги функций htons() htonl()
#define HTONS(x) \
    ((uint16_t)((x >> 8) | ((x << 8) & 0xff00)))
#define HTONL(x) \
    ((uint32_t)((x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | ((x << 24) & 0xff000000)))


//***********************************************************************************

enum {
    iUsr = 0,
    iLog,
    iLang
};

enum {
    idStop = 0,
    idStart,
    idBin,
    idText,
    idKey
};


enum {
    errNo = 0,   // "Ok",
    errExec,     // "Operation error",//"Ошибка выполнения операции",
    errRstFlash, // "FLASH reset error",//"Ошибка сброса FLASH",
    errWrFlash,  // "FLASH write error",//"Ошибка записи FLASH",
    errNoAck,    // "Error waiting for response",//"Ошибка ожидания ответа",
    errCmdReject,// "Command rejected",//"Команда отклонена",
    errSigInt,   // "Abort from keyboard",//"Процедура прервана с клавиатуры",
    errFatal,    // "Fatal error",//"Неисправимая ошибка",
    errFileLen,  // "Error fileLen",
    errFileOpen, // "Error fOpen",
    errGetMem,   // "Error getMem",
    errRdFile,   // "Error fRead",
    errWrFile,
    errUnknown   // "Unknown error"//Неизвестная ошибка"
};

enum {
    picClr = 0,
    picInfo,
    picTime,
    picWar,
    picErr,
    picCon,
    picDis,
    picOk
};

enum {
    gDeep = 0,
    gHumi,
    gTemp,
    gVolt,
    gTemp2,
    gTemp3,
    gNone
};

enum {
    qFifo = 0,
    qErr,
    qVolt,
    qDeep,
    qTemp,
    qHumi,
    qComp,
    qTemp2,
    qTemp3,
    qAccel,
    qGyro,
    qNone
};

enum {
    noneBit = 0,
    i2cBit = 1,
    kellerBit = 2,
    framBit = 4,
    tempBit = 8,
    framFullBit = 16,
    vsBit = 32,
    fifoBit = 64,
    unknownBit = 128
};
//
enum {
    jtime = 0,
    jms,
    jfifo,
    jerr,
    jvolt,
    jbmx,
    jqmc,
    jmpu
};
enum {
//    jbmx,
    jpress = 0,
    jtemp,
    jhumi
};
enum {
//    jqmc,
    jazimut = 0,
    jtemp2
};
enum {
//    jmpu,
    jstat,
    jtemp3,
    jaccel,
    jgyro
};
//

enum {
    tcpOk = 0,
    tcpTimeOut,
    tcpErr,
    tcpDone
};

//***********************************************************************************

#pragma pack(push,1)
typedef struct {
    void *graf;
    void *pic;
    void *cv;
} addr_graf_t;
#pragma pack(pop)



#pragma pack(push,1)
typedef struct {
    int cur;
    int max;
} fifo_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint32_t ms;
    fifo_t fifo;
    uint32_t err;
    float volt;
    float pres;
    float temp;
    float humi;
    float azimut;
    float temp2;
    float temp3;
    short accel[MAX_ACC_CNT];
    short gyro[MAX_GYR_CNT];
} qdata_t;
#pragma pack(pop)



//***********************************************************************************

#endif // DEFS_H
