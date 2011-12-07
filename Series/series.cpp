#include "series.h"

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

Episode* Series::GetEpisodeByOrder(EpisodeOrder order) const
{
    if (order.isSet())
        if (Season* season = GetSeason(order.season))
            return season->GetEpisode(order.episode);

    return NULL;
}

Episode* Series::GetNextEpisode() const
{
    if (!lSeasons_m.isEmpty())
        foreach(Season* season, lSeasons_m)
            if (!season->GetEpisodes().isEmpty())
                foreach(Episode* episode, season->GetEpisodes())
                    if (episode && !episode->GetAir().passed())
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

/***********************/
/***  PARSING START  ***/

QByteArray Series::GetContentInTag(const QByteArray& content, QByteArray tag, int start)
{
    int start_idx, end_idx, length;
    start_idx = end_idx = length = 0;
    int tag_start_idx = content.indexOf("<"+tag, start);
    if (tag_start_idx == -1)
        return "";

    start_idx = content.indexOf(">", tag.indexOf("<"+tag, start));
    end_idx = content.indexOf("</"+tag+">");
    length = end_idx - start_idx;
    if (length < -1)
        length = -1;

    return content.mid(start_idx, length);
}

void Series::ParseFromTV(const QByteArray& content)
{

}

void Series::ParseFromIMDB(const QByteArray& content)
{

}

/***   PARSING END   ***/
/***********************/
