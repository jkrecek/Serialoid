#ifndef SETTING_H
#define SETTING_H

#include <QStringList>
#include <QMap>
#include <QFile>
#include "spambot.h"

struct Setting
{
    Setting(QString fileName);
    void HandleSetting(QString line);
    Series* GetSeries(QString name) const { return lSeries_m.contains(name) ? lSeries_m[name] : NULL; }
    SeriesMap lSeries_m;
};

#endif // SETTING_H
