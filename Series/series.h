#ifndef SERIES_H
#define SERIES_H

#include <QMap>
#include <QStringList>
#include "episode.h"
#include "season.h"

typedef QMap<uint, Season*> SeasonMap;

class Series
{
    public:
        Series(QString name) : seriesName_m(name) {}

        void SetInfo(QString info) { info_m = info; }
        void SetTitles(QStringList l) { seriesTitles_m = l; }
        void SetCurrentSeason(uint c) { currentSeason_m = c; }

        SeasonMap& GetSeasons() { return lSeasons_m; }
        Season* GetSeason(uint Id) const { return lSeasons_m.contains(Id) ? lSeasons_m[Id] : NULL; }
        QStringList GetTitles() const { return seriesTitles_m; }
        QString GetMainTitle() const { return seriesTitles_m.empty() ? seriesName_m.trimmed() : seriesTitles_m.first().trimmed(); }
        QString GetName() const { return seriesName_m; }
        QString GetInfo() const { return info_m; }
        uint GetCurrentSeasonId() const { return currentSeason_m; }
        Season* GetCurrentSeason() const { return currentSeason_m && lSeasons_m.contains(currentSeason_m) ? lSeasons_m[currentSeason_m] : NULL; }
        Episode* GetEpisodeByOrder(EpisodeOrder order)
        {
            if (order.isSet())
                if (Season* season = GetSeason(order.season))
                    return season->GetEpisode(order.episode);

            return NULL;
        }

    private:
        uint currentSeason_m;
        QString seriesName_m;
        QStringList seriesTitles_m;
        QString info_m;
        SeasonMap lSeasons_m;
};

#endif // SERIES_H
