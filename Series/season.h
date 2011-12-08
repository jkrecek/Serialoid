#ifndef SEASON_H
#define SEASON_H

#include <QMap>
#include <QStringList>
#include "episode.h"

typedef QMap<uint, Episode*> EpisodeMap;

class Season
{
    public:
        Season(uint seasonNumber) : seasonNumber_m(seasonNumber) {}

        void SetInfo(QString info) { Info_m = info; }
        uint GetNumber() const { return seasonNumber_m; }

        QString GetInfo() const { return Info_m; }
        EpisodeMap& GetEpisodes() { return lEpisodes_m; }
        Episode* GetEpisode(uint Id) const { return lEpisodes_m.value(Id); }
    private:
        uint seasonNumber_m;
        QString Info_m;

        EpisodeMap lEpisodes_m;
};

#endif // SEASON_H
