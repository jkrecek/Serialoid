#ifndef SEASON_H
#define SEASON_H

#include <QMap>
#include <QStringList>
#include "episode.h"

typedef QMap<uint, Episode*> EpisodeMap;

class Season
{
    public:
        Season(uint _seasonId) : seasonId_m(_seasonId) {}

        void SetInfo(QString info) { Info_m = info; }

        const EpisodeMap& GetEpisodes()                 const { return lEpisodes_m; }
        uint GetNumber()                                const { return seasonId_m; }
        QString GetInfo()                               const { return Info_m; }
        Episode* GetEpisode(uint Id)                    const { return lEpisodes_m.value(Id); }
        Episode* GetOrAddEpisode(EpisodeOrder epOrder)
        {
            if (!epOrder.isSet())
                return NULL;

            Episode * _ep = GetEpisode(epOrder.episode);
            if (!_ep)
            {
                _ep = new Episode(epOrder);
                uint episodeId = epOrder.episode;
                lEpisodes_m.insert(episodeId, _ep);
            }

            return _ep;
        }
    private:
        uint seasonId_m;
        QString Info_m;

        EpisodeMap lEpisodes_m;
};

#endif // SEASON_H
