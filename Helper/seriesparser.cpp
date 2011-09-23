#include <QDebug>
#include <QTextStream>
#include "Formulas.h"
#include "seriesparser.h"

SeriesParser::SeriesParser(QString _setting, QString _error) : error_m(_error)
{
    QFile file(_setting);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!file.atEnd())
    {
        QString line = file.readLine();
        // allow putting comments into file
        if (!line.isNull() && !line.startsWith("#"))
            HandleSetting(line.trimmed());
    }
}

void SeriesParser::HandleSetting(QString line)
{
    QStringList setting = line.split(" ", QString::SkipEmptyParts);
    if (setting.empty())
        return;

    if (setting[0] == "create")
    {
        if (setting.size() == 2)
        {
            QString name = setting[1].trimmed();
            lSeries_m[name] = new Series(name);
            return;
        }
        writeError("Command 'create' has wrong number of parameters has wrong number of parameters (has "+QString::number(setting.size())+" instead of 4, did you used codename of series?)");
        return;
    }

    if (setting.size() < 3)
    {
        writeError("Globaly number of parameters must be 3 or higher!");
        return;
    }

    Series* series = GetSeries(setting[0]);
    if (!series)
    {
        writeError("Series codenamed '"+setting[0]+"' was not found!");
        return;
    }

    if (setting[1] == "info")
        series->SetInfo(line.mid(line.indexOf(setting[2])));
    else if (setting[1] == "titles")
        series->SetTitles(line.mid(line.indexOf(setting[2])).split(",", QString::SkipEmptyParts));
    else if (setting[1] == "season")
    {
        if (setting[2] == "add")
        {
            if (setting.size() == 4 || setting.size() == 5)
            {
                SeasonMap& seaM = series->GetSeasons();
                uint seasonId = setting[3].toUInt();
                if (!seasonId || series->GetSeason(seasonId))
                    return;

                Season* s = new Season(seasonId);
                s->SetHourdiff(setting.size() == 5 ? setting[4].toInt() : 0);
                seaM[seasonId] = s;
            }
            else
            {
                writeError("Command 'season add' for series codenamed '"+setting[0]+"' has wrong number of parameters (has "+QString::number(setting.size())+" instead of 4 or 5)");
                return;
            }
        }

        if (uint seasonId = setting[2].toUInt())
        {
            if (setting.size() > 5)
            {
                if (Season* season = series->GetSeason(seasonId))
                {
                    if (setting[3] == "info")
                        season->SetInfo(line.mid(line.indexOf(setting[4])));
                }
                else
                {
                    writeError("Command 'season seasonId' for series codenamed '"+setting[0]+"' unable to found season "+QString::number(seasonId));
                    return;
                }
            }
            else
            {
                writeError("Command 'season seasonId' for series codenamed '"+setting[0]+"' has wrong number of parameters (has "+QString::number(setting.size())+" instead of 5 or more)");
                return;
            }
        }
        return;
    }
    else
    {
        EpisodeOrder epO(setting[1]);
        if (epO.isSet())
        {
            if (setting.size() < 4)
            {
                writeError("Adding episode for series codenamed '"+setting[0]+"' has wrong number of parameters (has "+QString::number(setting.size())+" instead of 4 or more)");
                return;
            }

            Season* season = series->GetSeason(epO.season);
            if (!season)
            {
                writeError("Adding episode for series codenamed '"+setting[0]+"' unable to found season "+QString::number(epO.season));
                return;
            }

            EpisodeMap& episodes = season->GetEpisodes();
            Episode* episode = season->GetEpisode(epO.episode);
            if (!episode)
            {
                QStringList splitList = line.split("\"");
                QStringList stampPart = splitList[3].split(" ");

                episode = new Episode(epO);
                episode->SetName(splitList[1]);
                episode->SetAir(Timestamp(stampPart[0], stampPart[1], season->GetHourDiff()));
                episodes[epO.episode] = episode;
            }
            else
            {
                if (setting[2] == "info")
                    episode->SetInfo(line.mid(line.indexOf(setting[3])));
            }
        }
    }
}

void SeriesParser::writeError(QString error)
{
    QFile file(error_m);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream stream(&file);
    stream << "ERROR: " << Timestamp().write(FORMAT_TIME_DATE) << " " << error << endl;
    file.close();
}
