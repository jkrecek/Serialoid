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
        void SetHourdiff(int diff) { airHourDiff_m = diff; }

        uint GetNumber() const { return seasonNumber_m; }
        int GetHourDiff() const { return airHourDiff_m; }
        int GetSecondsDiff() const { return airHourDiff_m*3600; }
        QString GetInfo() const { return Info_m; }
        EpisodeMap& GetEpisodes() { return lEpisodes_m; }
        Episode* GetEpisode(uint Id) const { return lEpisodes_m.contains(Id) ? lEpisodes_m[Id] : NULL; }
    private:
        uint seasonNumber_m;
        QString Info_m;
        int airHourDiff_m;

        EpisodeMap lEpisodes_m;
};

#endif // SEASON_H
