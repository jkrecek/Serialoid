#ifndef SERIES_H
#define SERIES_H

#include <QMap>
#include <QStringList>
#include "episode.h"
#include "season.h"

enum Site
{
    SITE_IMDB       = 0,
    SITE_TV         = 1,

    SITE_ERROR      = 99
};

#define SITE_COUNT 2

typedef QMap<uint, Season*> SeasonMap;
typedef QList<Episode*> EpisodeList;
typedef QList<QString> LinkList;

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
        void AddLink(QString _l) { links_m.push_back(_l); }

        SeasonMap& GetSeasons() { return lSeasons_m; }
        Season* GetSeason(uint Id) const { return lSeasons_m.value(Id); }
        QStringList GetTitles() const { return seriesTitles_m; }
        QString GetMainTitle() const { return seriesTitles_m.empty() ? seriesName_m : seriesTitles_m.first(); }
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

        EpisodeList GetAllEpisodes()
        {
            EpisodeList allEp;
            if (!lSeasons_m.isEmpty())
                foreach(Season* season, lSeasons_m)
                    if (!season->GetEpisodes().isEmpty())
                        foreach(Episode* episode, season->GetEpisodes())
                            if (episode)
                                allEp.push_back(episode);

            return allEp;
        }
        LinkList GetLinks() const { return links_m; }

    private:
        QString seriesName_m;
        QStringList seriesTitles_m;
        QString info_m;
        SeasonMap lSeasons_m;
        LinkList links_m;
};

#endif // SERIES_H
