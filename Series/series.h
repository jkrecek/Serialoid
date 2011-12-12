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

//      season_id  season class
typedef QMap<uint, Season*> SeasonMap;

typedef QList<Episode*> EpisodeList;
typedef QList<QString> LinkList;

class Series
{
    public:
        Series(QString name);
        ~Series();

        void SetInfo(QString info);
        void SetTitles(QStringList l);
        void AddLink(QString _l);
        void SetAir(QString unix_time, int gmt_diff);

        const SeasonMap& GetSeasons()   const { return lSeasons_m; }
        QStringList& GetTitles()              { return seriesTitles_m; }
        Season* GetSeason(uint Id)      const { return lSeasons_m.value(Id); }
        QString GetMainTitle()          const { return seriesTitles_m.empty() ? seriesName_m : seriesTitles_m.first(); }
        QString GetName()               const { return seriesName_m; }
        QString GetInfo()               const { return info_m; }
        LinkList GetLinks()             const { return links_m; }
        Timestamp GetDailyAir()         const { return airHour_m; }

        Episode* GetEpisodeByOrder(EpisodeOrder epOrder) const;
        Episode* GetNextEpisode() const;
        EpisodeList GetAllEpisodes();
        Episode* GetOrAddEpisode(EpisodeOrder epOrder);
        Season* GetOrAddSeason(uint Id);

        /***********************/
        /***  PARSING START  ***/

        // basic helpers for parsing
        QByteArray GetSimpleContent(const QByteArray& content, QByteArray tag, int start = 0, bool forward = true);
        QByteArray GetAdvancedContent(const QByteArray& content, QByteArray tag, int& idx);
        //QByteArray GetAllTags(const QByteArray& content, QByteArray tag);

        QString GetTitleFromHead(const QByteArray& content, Site site);

        void ParseFromTV(const QByteArray& content);
        void ParseFromIMDB(const QByteArray& content);

        /***   PARSING END   ***/
        /***********************/

    private:
        QString     seriesName_m;
        QStringList seriesTitles_m;
        QString     info_m;
        SeasonMap   lSeasons_m;
        LinkList    links_m;
        Timestamp   airHour_m;
};

#endif // SERIES_H
