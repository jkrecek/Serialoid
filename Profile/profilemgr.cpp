#include <QFile>
#include <QTextStream>
#include "profilemgr.h"
#include "timestamp.h"

void ProfileMgr::Load(QString _profiles, QString _error)
{
    if (!_profiles.isNull())
        fProfiles_m = _profiles;
    if (!_error.isNull())
        fError_m = _error;

    if (fProfiles_m.isNull())
        return;

    QFile file(fProfiles_m);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        PrintOut(now_time, "Profile file NOT found");
        return;
    }

    PrintOut(now_time, "Profile file found");

    while (!file.atEnd())
    {
        QString line = file.readLine();
        if (line.isNull())
            continue;

        QStringList linePart = line.split(" ");
        if (linePart.size() < 3)
        {
            writeError("Line '"+line+"' cannot have size less then 3");
            continue;
        }
        QString& profileName = linePart[0];
        if (lProfiles_m.contains(profileName))
        {
            writeError("Profile "+profileName+" already exists!");
            continue;
        }

        Profile* profile = new Profile(profileName);
        profile->SetPassHash(linePart[1]);
        profile->SetSeries(line.split("\"")[1]);
        lProfiles_m[profileName] = profile;
    }
}

void ProfileMgr::Save()
{
    QFile file(fProfiles_m);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return;

    if (lProfiles_m.isEmpty())
        return;

    QTextStream out(&file);
    foreach(Profile* profile, lProfiles_m)
    {
        if (profile->GetProfileSeries().isEmpty())
            continue;

        out << profile->GetName() << " " << profile->GetPassHash() << " \"";

        bool first = true;
        foreach(Series* series, profile->GetProfileSeries())
        {
            if (!first)
                out << ", ";
            else
                first = false;
            out << series->GetName();
        }

        out << "\"" << endl;
    }
    file.close();
}

void ProfileMgr::AddProfile(Profile* prof)
{
    lProfiles_m.insert(prof->GetName(), prof);
    Save();
}

bool ProfileMgr::isNameForbidden(QString profileName) const
{
    return profileName.startsWith("profile") || profileName.startsWith("add") ||
            profileName.contains("\"") || profileName.contains(",") || profileName.contains("*");
}

void ProfileMgr::writeError(QString error)
{
    QFile file(fError_m);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream stream(&file);
    stream << "ERROR(profile): " << Timestamp().write(FORMAT_TIME_DATE) << " " << error << endl;
    file.close();
}
