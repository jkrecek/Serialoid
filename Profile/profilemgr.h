#ifndef PROFILEMGR_H
#define PROFILEMGR_H

#include <QMap>
#include <QStringList>
#include "profile.h"
#include "spambot.h"
#include "series.h"

typedef QMap<QString, Series*> SeriesMap;
typedef QMap<QString, Profile*> ProfileMap;

class ProfileMgr
{
    public:
        ProfileMgr(QString _fileName, QString _errorFileName, SeriesMap& _series);

        bool isNameForbidden(QString profileName) const;
        void writeError(QString error);
        bool HasProfile(QString name) const { return lProfiles_m.contains(name); }
        Profile* GetProfile(QString name) const { return lProfiles_m.value(name); }
        void AddProfile(Profile* prof);

        void Save() { _save(); }

    private:
        void _save();

        QString fileName_m;
        QString errorFileName_m;
        const SeriesMap& lSeries_m;
        ProfileMap lProfiles_m;
};

#endif // PROFILEMGR_H
