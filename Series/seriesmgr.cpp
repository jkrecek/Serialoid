#include "Formulas.h"
#include "seriesmgr.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

void SeriesMgr::Load(QString _series, QString _error)
{
    lSeries_m.clear();          // for reload purposes

    if (!_series.isNull())
        fSeries_m = _series;
    if (!_error.isNull())
        fError_m = _error;

    if (fSeries_m.isNull())
        return;

    QFile file(fSeries_m);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        PrintOut(now_time, "Series file NOT found");
        return;
    }
    PrintOut(now_time, "Series file found");

    while (!file.atEnd())
    {
        QString line = file.readLine();
        // allow putting comments into file
        if (line.isNull() || line.startsWith("#"))
            continue;

        QStringList setting = line.split(" ", QString::SkipEmptyParts);
        if (setting.empty())
            continue;

        if (setting[0] == "create")
        {
            if (setting.size() == 2)
            {
                QString name = setting[1].trimmed();
                lSeries_m[name] = new Series(name);
                continue;
            }
            writeError("Command 'create' has wrong number of parameters has wrong number of parameters (has "+QString::number(setting.size())+" instead of 4, did you used codename of series?)");
            continue;
        }

        if (setting.size() < 3)
        {
            writeError("Globaly number of parameters must be 3 or higher!");
            continue;
        }

        Series* series = lSeries_m.value(setting[0]);
        if (!series)
        {
            writeError("Series codenamed '"+setting[0]+"' was not found!");
            continue;
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
                        continue;

                    Season* s = new Season(seasonId);
                    s->SetHourdiff(setting.size() == 5 ? setting[4].toInt() : 0);
                    seaM[seasonId] = s;
                }
                else
                {
                    writeError("Command 'season add' for series codenamed '"+setting[0]+"' has wrong number of parameters (has "+QString::number(setting.size())+" instead of 4 or 5)");
                    continue;
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
                        continue;
                    }
                }
                else
                {
                    writeError("Command 'season seasonId' for series codenamed '"+setting[0]+"' has wrong number of parameters (has "+QString::number(setting.size())+" instead of 5 or more)");
                    continue;
                }
            }
            continue;
        }
        else
        {
            EpisodeOrder epO(setting[1]);
            if (epO.isSet())
            {
                if (setting.size() < 4)
                {
                    writeError("Adding episode for series codenamed '"+setting[0]+"' has wrong number of parameters (has "+QString::number(setting.size())+" instead of 4 or more)");
                    continue;
                }

                Season* season = series->GetSeason(epO.season);
                if (!season)
                {
                    writeError("Adding episode for series codenamed '"+setting[0]+"' unable to found season "+QString::number(epO.season));
                    continue;
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
}

void SeriesMgr::writeError(QString error)
{
    if (fError_m.isNull())
        return;

    QFile file(fError_m);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream stream(&file);
    stream << "ERROR(series parser): " << Timestamp().write(FORMAT_TIME_DATE) << " " << error << endl;
    file.close();
}
