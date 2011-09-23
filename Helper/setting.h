#ifndef SETTING_H
#define SETTING_H

#include <QFile>
#include <QMap>
#include <QStringList>
#include "spambot.h"

struct Setting
{
    Setting(QString _setting, QString _error);
    void HandleSetting(QString line);
    Series* GetSeries(QString name) const { return lSeries_m.contains(name) ? lSeries_m[name] : NULL; }
    SeriesMap lSeries_m;
    void writeError(QString error);

    private:
        QString error_m;
};

#endif // SETTING_H
