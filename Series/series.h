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
        void SetTitles(QStringList l)
        {
            seriesTitles_m.clear();
            foreach(QString s, l)
                seriesTitles_m.push_back(s.trimmed());
        }

        SeasonMap& GetSeasons() { return lSeasons_m; }
        Season* GetSeason(uint Id) const { return lSeasons_m.contains(Id) ? lSeasons_m[Id] : NULL; }
        QStringList GetTitles() const { return seriesTitles_m; }
        QString GetMainTitle() const { return seriesTitles_m.empty() ? seriesName_m.trimmed() : seriesTitles_m.first().trimmed(); }
        QString GetName() const { return seriesName_m; }
        QString GetInfo() const { return info_m; }
        Episode* GetEpisodeByOrder(EpisodeOrder order)
        {
            if (order.isSet())
                if (Season* season = GetSeason(order.season))
                    return season->GetEpisode(order.episode);

            return NULL;
        }
        Episode* GetNextEpisode() const
        {
            if (!lSeasons_m.isEmpty())
                foreach(Season* season, lSeasons_m)
                    if (!season->GetEpisodes().isEmpty())
                        foreach(Episode* episode, season->GetEpisodes())
                            if (episode && !episode->GetAir().passed())
                                return episode;
            return NULL;
        }

    private:
        QString seriesName_m;
        QStringList seriesTitles_m;
        QString info_m;
        SeasonMap lSeasons_m;
};

#endif // SERIES_H
