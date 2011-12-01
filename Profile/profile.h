#ifndef PROFILE_H
#define PROFILE_H

#include <QStringList>
#include "Formulas.h"
#include "series.h"
#include "seriesmgr.h"

typedef QMap<QString, Series*> SeriesMap;

class Profile
{
    public:
        Profile(QString _name) : profileName_m(_name) {}

        void SetSeries(QString seriesString)
        {
            lSeries_m.clear();
            QStringList list = seriesString.split(",");
            foreach(QString listPart, list)
            {
                QString seriesName = listPart.trimmed();
                if (!sSeries.GetMap().contains(seriesName) || lSeries_m.contains(seriesName))
                    continue;

                lSeries_m.insert(seriesName, sSeries.GetMap().value(seriesName));
            }
        }
        void SetPassHash(QString hash) { passwordHash_m = hash; }
        bool IsPassCorrect(QString hash) const { return !hash.isEmpty() && getHashFor(hash) == passwordHash_m; }
        QString GetPassHash() const { return passwordHash_m; }

        QString GetName() const { return profileName_m; }
        SeriesMap GetProfileSeries() const { return lSeries_m; }

    private:
        SeriesMap lSeries_m;
        QString profileName_m;
        QString passwordHash_m;
};

#endif // PROFILE_H
