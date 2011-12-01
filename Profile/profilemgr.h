#ifndef PROFILEMGR_H
#define PROFILEMGR_H

#include <QMap>
#include <QStringList>
#include "profile.h"
#include "spambot.h"
#include "series.h"
#include "singleton.h"

typedef QMap<QString, Series*> SeriesMap;
typedef QMap<QString, Profile*> ProfileMap;

class ProfileMgr
{
    public:
        ProfileMgr() {}
        ~ProfileMgr() {}

        void Load(QString _fileName, QString _errorFileName);
        void Save();

        bool isNameForbidden(QString profileName) const;
        void writeError(QString error);
        bool HasProfile(QString name) const { return lProfiles_m.contains(name); }
        Profile* GetProfile(QString name) const { return lProfiles_m.value(name); }
        void AddProfile(Profile* prof);

    private:
        ProfileMap lProfiles_m;

        QString fProfiles_m;
        QString fError_m;
};

#define sProfile Singleton<ProfileMgr>::Instance()

#endif // PROFILEMGR_H
