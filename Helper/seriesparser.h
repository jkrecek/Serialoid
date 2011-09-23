#ifndef SETTING_H
#define SETTING_H

#include <QFile>
#include <QMap>
#include <QStringList>
#include "spambot.h"

struct SeriesParser
{
    SeriesParser(QString _setting, QString _error, SeriesMap& _lSeries);
    void HandleSetting(QString line);
    Series* GetSeries(QString name) const { return lSeries_m.value(name); }
    void writeError(QString error);

    private:
        QString error_m;
        SeriesMap& lSeries_m;
};

#endif // SETTING_H
