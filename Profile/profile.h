#ifndef PROFILE_H
#define PROFILE_H

#include <QDebug>
#include <QStringList>
#include "series.h"

typedef QMap<QString, Series*> SeriesMap;

class Profile
{
    public:
        Profile(QString _name) : profileName_m(_name) {}

        void SetSeries(QString seriesString, const SeriesMap& _lSeries)
        {
            QStringList list = seriesString.split(",");
            foreach(QString listPart, list)
            {
                QString seriesName = listPart.trimmed();
                if (!_lSeries.contains(seriesName) || lSeries_m.contains(seriesName))
                    continue;

                lSeries_m.insert(seriesName, _lSeries.value(seriesName));
            }
        }

        QString GetName() const { return profileName_m; }
        SeriesMap GetProfileSeries() const { return lSeries_m; }

    private:
        SeriesMap lSeries_m;
        QString profileName_m;
};

#endif // PROFILE_H
