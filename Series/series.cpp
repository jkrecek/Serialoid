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

void Series::ParseFromTV(const QByteArray& content)
{
    static Site site = SITE_TV;
    // get title
    QString titleTitle = GetTitleFromHead(content,site);
    if (!seriesTitles_m.contains(titleTitle))
        seriesTitles_m.push_front(titleTitle);

    int idx = 0;
    while (idx != -1)
    {
        QByteArray arr = GetAdvancedContent(content, "div class=\"info\"", idx);
        if (arr.isEmpty())
            break;

        QByteArray sub = GetSimpleContent(arr, "div class=\"meta\"",0);
        int idxSeason = sub.indexOf(Constant::se)+Constant::se.size() + 1;
        int idxEpisode = sub.indexOf(Constant::ep)+Constant::ep.size() + 1;
        EpisodeOrder epOrder(sub.mid(idxSeason, sub.indexOf(Constant::comma, idxSeason) - idxSeason).toInt(), sub.mid(idxEpisode, sub.indexOf(Constant::newline, idxEpisode) - idxEpisode).toInt());

        Season* _season = GetSeason(epOrder.season);
        if (!_season)
        {
            _season = new Season(epOrder.season);
            lSeasons_m.insert(epOrder.season, _season);
        }

        Episode* episode = _season->GetEpisode(epOrder.episode);
        if (!episode)
        {
            episode = new Episode(epOrder);
            _season->GetEpisodes().insert(epOrder.episode, episode);
        }

        episode->SetName(GetSimpleContent(arr, "a href=\""));
        episode->SetInfo(GetSimpleContent(arr, "p class=\"synopsis\""));

        QStringList dateArr = QString(sub.mid(sub.indexOf(Constant::air)+Constant::air.size()+1, -1)).split("/");
        episode->SetAir(Timestamp(dateArr.value(1)+"."+dateArr.value(0)+"."+dateArr.value(2), GetDailyAir().getTime(), GetDailyAir().getGMT()));

        episode->SetRating(GetSimpleContent(content, "span class=\"number\"", idx, false).toFloat());
    }
}

void Series::ParseFromIMDB(const QByteArray& content)
{

}


/***** HELPERS *****/

QByteArray Series::GetSimpleContent(const QByteArray &content, QByteArray fullTag, int start, bool forward)
{
    int itsStart;
    if (forward)
        itsStart = content.indexOf("<"+fullTag, start);         // index of opening tag begin, default look forward
    else
        itsStart = content.lastIndexOf("<"+fullTag, start);     // index of opening tag begin, look backwards

    if (itsStart == -1)                                         // start tag not found while searching from start point
        return QByteArray();

    int itsEnd = content.indexOf(">", itsStart)+1;              // index of opening tag end
    QByteArray simpleTag = (fullTag.contains(" ") ? fullTag.left(fullTag.indexOf(" ")) : fullTag);
    int iteStart = content.indexOf("</"+simpleTag+">", itsEnd); // index of closing tag start
    int length = iteStart - itsEnd;                             // difference between itsEnd and iteStart, aka length of desired string
    if (length < -1)                                            // should not never happend
        length = -1;

    return content.mid(itsEnd, length).trimmed();
}

//QByteArray Series::GetAllTags(const QByteArray &content, QByteArray tag)
//{
//    QMap<int,QString> m_map;
//    int from = 0;
//    // find all starting tags
//    int itsStart;
//    while(from != -1)
//    {
//        itsStart = content.indexOf("<"+tag, from);
//        if (itsStart == -1)
//            break;
//        m_map.insert(itsStart, content.mid(itsStart, content.indexOf(">", itsStart)-itsStart+1));
//        from = itsStart+1;
//    }
//    // find all ending tags
//    from = 0;
//    while(from != -1)
//    {
//        itsStart = content.indexOf("</"+tag+">", from);
//        if (itsStart == -1)
//            break;
//        m_map.insert(itsStart, "</"+tag+">");
//        from = itsStart + 1;
//    }
//
//    for(QMap<int,QString>::Iterator itr = m_map.begin(); itr != m_map.end(); ++itr)
//        qDebug << "Idx: '"+QString::number(itr.key())+"'' Value: '"+itr.value()+"'";
//
//    return QByteArray();
//}

QByteArray Series::GetAdvancedContent(const QByteArray &content, QByteArray fullTag, int& idx)
{
    idx = content.indexOf("<"+fullTag, idx);
    if (idx == -1)
        return QByteArray();

    QByteArray simpleTag = fullTag.left(fullTag.indexOf(" "));
    int sEndIdx = content.indexOf(">", idx)+1;
    uint currentOpen = 1;
    while(currentOpen && idx != -1)
    {
        idx += simpleTag.size(); // increase index so we shouldnt found same tag

        idx = content.indexOf(simpleTag, idx);
        if (content.at(idx-1) == Constant::tagStart)
            ++currentOpen;
        else if (content.at(idx-2) == Constant::tagStart && content.at(idx-1) == Constant::slash)
            --currentOpen;
    }

    if (currentOpen)
        return QByteArray();

    return content.mid(sEndIdx, idx-sEndIdx-2);
}

QString Series::GetTitleFromHead(const QByteArray& content, Site site)
{
    // get title byte array and convert it into QString
    QString title = GetSimpleContent(content, "title");

    // now strip it of useless data
    switch(site)
    {
        case SITE_IMDB:
            title.remove("IMDb - ", Qt::CaseInsensitive);
            title.remove(title.indexOf("(", 0, Qt::CaseInsensitive), title.size());
            break;
        case SITE_TV:
            title.remove(" - TV.com", Qt::CaseInsensitive);
            if (title.contains("Season", Qt::CaseInsensitive))
                title = title.left(title.indexOf("Season 1-", Qt::CaseInsensitive)).trimmed();
            break;
        default:
            break;
    }

    return title.trimmed();
}

/***   PARSING END   ***/
/***********************/
