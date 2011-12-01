#ifndef FORMULAS_H
#define FORMULAS_H

#include <QStringList>
#include <QCryptographicHash>
#include <time.h>
#include <stdio.h>

#define FIRST_YEAR 1970
#define FIRST_MONTH 1
#define FIRST_DAY 1

#define COMPARE "compare"
#define SERIES "series"
#define PROFILE "profile"
#define HASH "hash"
#define LIST "list"
#define RELOAD "reload"
#define NEXT "next"
#define INFO "info"
#define TITLES "titles"

#define BOT_GMT 1
#define IS_DST true

#define DAY_S 86400
#define DAY_H 24
#define HOUR_S 3600
#define MINUTE_S 60

inline uint GetDaysInMonth(int month, int year)
{
    switch(month)
    {
        case 2:
            return year%4 == 0 ? 29 : 28;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        default:
            return 0;
    }
}

inline uint GetDaysForYear(int year)
{
    return year%4 == 0 ? 366 : 365;
}

inline bool isInRange(int min, int val, int max)
{
    return min <= val && val <= max;
}

inline QString getHashFor(QString pass)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(pass.toStdString().c_str(), pass.size());
    return hash.result().toHex();
}

inline bool isSha1Hash(QString hash)
{
    QByteArray arr = hash.toUtf8();
    if (arr.size() != 40)
        return false;

    if (arr.contains(" "))
        return false;

    return true;
}

inline void PrintOut(QString nowTime, QString str)
{
    printf("%s: %s\n", nowTime.toStdString().c_str(), str.toStdString().c_str());
}

#endif // FORMULAS_H
