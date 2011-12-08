#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <QStringList>
#include <time.h>
#include "Formulas.h"

#define now_time Timestamp().write(FORMAT_TIME)

enum Format
{
    FORMAT_DAY_S_TIME_DATE = 0,
    FORMAT_DAY_S_DATE_TIME,
    FORMAT_DAY_S_TIME_DATE_GMT,
    FORMAT_DAY_S_DATE_TIME_GMT,
    FORMAT_DAY_L_TIME_DATE,
    FORMAT_DAY_L_DATE_TIME,
    FORMAT_TIME_DATE,
    FORMAT_DATE_TIME,
    FORMAT_TIME,
    FORMAT_DATE,
    FORMAT_UNIX
};

class Timestamp
{
    public:
        Timestamp(int _unix = time(0), int _diff = BOT_GMT);
        Timestamp(QString _str1, QString _str2, int _diff = BOT_GMT);

        uint workUnix()     const { return unix_m + (gmtDiff_m + IS_DST) * HOUR_S; }

        bool isCorrect()    const { return unix_m != 0; }
        bool Aired()        const { return isCorrect() && unix_m < time(0); }
        bool toBeAired()    const { return isCorrect() && unix_m > time(0); }

        uint timeTo()       const { return toBeAired() ? unix_m - time(0) : INT_MAX; }
        uint timeFrom()     const { return     Aired() ? time(0) - unix_m : INT_MAX; }

        QString writeGMT()  const;
        QString correctTimeValue(uint val) const;

        QString write(Format f = FORMAT_UNIX) const;
        QString getTime() const;
        QString getDate() const;
        QString getDay(int pos = -1) const;

        int getUnix()   const { return unix_m; }
        int getGMT()    const { return gmtDiff_m; }

        QString getTo() const;

    private:
        int unix_m;
        int gmtDiff_m;
};

#endif // TIMESTAMP_H
