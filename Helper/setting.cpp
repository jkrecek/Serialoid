#include "setting.h"
#include "Formulas.h"
#include <QDebug>

Setting::Setting(QString fileName)
{
    QFile file(fileName);
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

void Setting::HandleSetting(QString line)
{
    QStringList setting = line.split(" ", QString::SkipEmptyParts);
    if (setting.empty())
        return;

    if (setting[0] == "create")
    {
        if (setting.size() == 2)
            lSeries_m[setting[1].trimmed()] = new Series(setting[1].trimmed());
        return;
    }

    if (setting.size() <= 2)
        return;

    Series* series = GetSeries(setting[0]);
    if (!series)
        return;

    if (setting[1] == "info")
    {
        series->SetInfo(line.mid(line.indexOf(setting[2])));
        return;
    }

    if (setting[1] == "titles")
    {
        series->SetTitles(line.mid(line.indexOf(setting[2])).split(",", QString::SkipEmptyParts));
        return;
    }

    if (setting[1] == "season")
    {
        if (setting[2] == "current")
        {
            if (setting.size() == 4)
                if (uint seasonNumber = setting[3].toUInt())
                    series->SetCurrentSeason(seasonNumber);
            return;
        }

        if (setting[2] == "add")
        {
            if (setting.size() < 4)
                return;

            SeasonMap& seaM = series->GetSeasons();
            uint seasonId = setting[3].toUInt();
            if (!seasonId || series->GetSeason(seasonId))
                return;

            Season* s = new Season(seasonId);
            s->SetHourdiff(setting.size() == 5 ? setting[4].toInt() : 0);
            seaM[seasonId] = s;
        }

        if (uint seasonId = setting[2].toUInt())
        {
            if (setting.size() > 5)
                if (Season* season = series->GetSeason(seasonId))
                    if (setting[3] == "info")
                        season->SetInfo(line.mid(line.indexOf(setting[4])));
            return;
        }
        return;
    }

    EpisodeOrder epO(setting[1]);
    if (epO.isSet())
    {
        if (setting.size() < 4)
            return;

        Season* season = series->GetSeason(epO.season);
        if (!season)
            return;

        EpisodeMap& episodes = season->GetEpisodes();
        if (Episode* episode = season->GetEpisode(epO.episode))
        {
            if (setting[2] == "info")
                episode->SetInfo(line.mid(line.indexOf(setting[3])));
            return;
        }

        Episode* episode = new Episode(epO.episode);
        QStringList splitList = line.split("\"");
        episode->SetName(splitList[1]);

        QStringList stampPart = splitList[3].split(" ");
        episode->SetAir(Timestamp(stampPart[0], stampPart[1], season->GetHourDiff()));

        episode->SetEpisodeOrder(epO);
        episodes[epO.episode] = episode;
    }
}
