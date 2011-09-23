#include <QFile>
#include <QTextStream>
#include "profilemgr.h"
#include "timestamp.h"

ProfileMgr::ProfileMgr(QString _fileName, QString _errorFileName, SeriesMap& _series) :
    fileName_m(_fileName), errorFileName_m(_errorFileName), lSeries_m(_series)
{
    QFile file(fileName_m);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!file.atEnd())
    {
        QString line = file.readLine();
        if (line.isNull())
            continue;

        QStringList linePart = line.split(" ");
        if (linePart.size() < 2)
        {
            writeError("Line '"+line+"' cannot have size less then 2");
            continue;
        }
        QString& profileName = linePart[0];
        if (lProfiles_m.contains(profileName))
        {
            writeError("Profile "+profileName+" already exists!");
            continue;
        }

        Profile* profile = new Profile(profileName);
        profile->SetSeries(line.split("\"")[1], lSeries_m);
        lProfiles_m[profileName] = profile;
    }
}

void ProfileMgr::_save()
{
    QFile file(fileName_m);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return;

    if (lProfiles_m.isEmpty())
        return;

    QTextStream out(&file);
    foreach(Profile* profile, lProfiles_m)
    {
        if (profile->GetProfileSeries().isEmpty())
            continue;

        out << profile->GetName() << " \"";

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
    _save();
}

bool ProfileMgr::isNameForbidden(QString profileName) const
{
    return profileName.startsWith("profile") || profileName.startsWith("add") ||
           profileName.contains("\"") || profileName.contains(",");
}

void ProfileMgr::writeError(QString error)
{
    QFile file(errorFileName_m);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream stream(&file);
    stream << "ERROR(profile): " << Timestamp().write(FORMAT_TIME_DATE) << " " << error << endl;
    file.close();
}
