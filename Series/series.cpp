#include "series.h"
#include <QMap>

Series::Series(QString name) : seriesName_m(name)
{
}

void Series::SetInfo(QString info)
{
    info_m = info;
}

void Series::SetTitles(QStringList l)
{
    seriesTitles_m.clear();
    foreach(QString s, l)
        seriesTitles_m.push_back(s.trimmed());
}

void Series::AddLink(QString _l)
{
    links_m.push_back(_l);
}

void Series::SetAir(QString unix, int gmt_diff)
{
    airHour_m = Timestamp("0.0.0", unix, gmt_diff);
}

Episode* Series::GetEpisodeByOrder(EpisodeOrder order) const
{
    if (order.isSet())
        if (Season* season = GetSeason(order.season))
            return season->GetEpisode(order.episode);

    return NULL;
}

Season* Series::GetOrAddSeason(uint Id)
{
    if (!Id)
        return NULL;

    Season * _s = lSeasons_m.value(Id);
    if (!_s)
    {
        _s = new Season(Id);
        lSeasons_m.insert(Id, _s);
    }

    return _s;
}

Episode* Series::GetOrAddEpisode(EpisodeOrder epOrder)
{
    return epOrder.isSet() ? GetOrAddSeason(epOrder.season)->GetOrAddEpisode(epOrder) : NULL;
}

Episode* Series::GetNextEpisode() const
{
    if (!lSeasons_m.isEmpty())
        foreach(Season* season, lSeasons_m)
            if (!season->GetEpisodes().isEmpty())
                foreach(Episode* episode, season->GetEpisodes())
                    if (episode && episode->GetAir().toBeAired())
                        return episode;
    return NULL;
}

EpisodeList Series::GetAllEpisodes()
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
