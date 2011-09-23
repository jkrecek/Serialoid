#include "timestamp.h"
#include "Formulas.h"
#include <QDebug>

Timestamp::Timestamp(uint _unix, int _diff) : unix_m(_unix), gmtDiff_m(_diff)
{
}

Timestamp::Timestamp(QString _str1, QString _str2, int _diff) : unix_m(0), gmtDiff_m(_diff)
{
    QString timeString;
    QString dateString;
    if (_str1.contains(":") && _str2.contains("."))
    {
        timeString = _str1;
        dateString = _str2;
    }
    else if (_str1.contains(".") && _str2.contains(":"))
    {
        timeString = _str2;
        dateString = _str1;
    }
    else
        return;

    uint _unix = 0;

    QStringList dateList = dateString.split(".");
    if (dateList.size() == 3)
    {
        uint day = dateList[0].toUInt();
        uint month = dateList[1].toUInt();
        uint year = dateList[2].toUInt();

        if (isInRange(1, day, 31) && isInRange(1, month, 12) && year > 1970)
        {
            for(uint y = FIRST_YEAR; y != year; ++y)
                _unix += GetDaysForYear(y);

            for(uint m = FIRST_MONTH; m != month; ++m)
                _unix += GetDaysInMonth(m, year);

            // date starts from 1
            _unix += day-1;

            // days so
            _unix *= 86400;
        }
    }

    QStringList timeList = timeString.split(":");
    if (timeList.size() == 2 || timeList.size() == 3)
    {
        uint hour = timeList[0].toUInt();
        uint minute = timeList[1].toUInt();
        uint second = timeList.size() == 3 ? timeList[2].toInt() : 0;
        if (isInRange(0, hour, 23) && isInRange(0, minute, 60) && isInRange(0, second, 60))
        {
            _unix += 3600*hour;
            _unix += 60*minute;
            _unix += second;
        }
    }

    _unix -= (_diff+IS_DST)*HOUR_S;

    qDebug() << _unix;
    qDebug() << time(0);
    unix_m = _unix;
}

QString Timestamp::writeGMT() const
{
    QString look = "GMT";
    if (gmtDiff_m >= 0)
        look.append("+");
    look.append(QString::number(gmtDiff_m));
    return look;
}

QString Timestamp::correctTimeValue(uint val) const
{
    QString str = QString::number(val);
    if (val < 10)
        str.prepend("0");
    return str;
}

QString Timestamp::getTime() const
{
    uint hours = (workUnix()/HOUR_S)%DAY_H;
    uint minutes = (workUnix()/MINUTE_S)%MINUTE_S;
    uint seconds = workUnix()%MINUTE_S;

    return QString::number(hours)+":"+correctTimeValue(minutes)+":"+correctTimeValue(seconds);
}

QString Timestamp::getDate() const
{
    uint days = workUnix()/86400;

    uint year = FIRST_YEAR;
    for(; days >= GetDaysForYear(year); ++year)
        days -= GetDaysForYear(year);

    uint month = FIRST_MONTH;
    for(; month <= 12 && days >= GetDaysInMonth(month, year); ++month)
        days -= GetDaysInMonth(month, year);

    return QString::number(days+1)+"."+QString::number(month)+"."+QString::number(year);
}

QString Timestamp::getDay(int pos) const
{
    QString day;
    switch((workUnix()/86400)%7)
    {
        case 0: day = "Thursday"; break;
        case 1: day = "Friday"; break;
        case 2: day = "Saturday"; break;
        case 3: day = "Sunday"; break;
        case 4: day = "Monday"; break;
        case 5: day = "Tuesday"; break;
        case 6: day = "Wednesday"; break;
    }

    return day.left(pos);
}

QString Timestamp::write(Format f) const
{
    switch(f)
    {
        case FORMAT_DAY_S_TIME_DATE:    return getDay(3)+" "+getTime()+" "+getDate();
        case FORMAT_DAY_S_DATE_TIME:    return getDay(3)+" "+getDate()+" "+getTime();
        case FORMAT_DAY_L_TIME_DATE:    return getDay()+" "+getTime()+" "+getDate();
        case FORMAT_DAY_L_DATE_TIME:    return getDay()+" "+getDate()+" "+getTime();
        case FORMAT_TIME_DATE:          return getTime()+" "+getDate();
        case FORMAT_DATE_TIME:          return getDate()+" "+getTime();
        case FORMAT_TIME:               return getTime();
        case FORMAT_DATE:               return getDate();
        case FORMAT_UNIX:               return QString::number(unix_m);
        default:                        return QString::null;
    }
}

QString Timestamp::getTo() const
{
    uint c = timeTo();
    qDebug() << c;
    uint days = c/86400;
    uint hours = (c/3600)%24;
    uint minutes = (c/60)%60;
    uint seconds = (c)%60;

    QString targetString = QString::number(hours)+":"+correctTimeValue(minutes)+":"+correctTimeValue(seconds);
    if (days)
        targetString.prepend(QString::number(days)+" "+(days == 1 ? "day" : "days")+" ");

    return targetString;
}
