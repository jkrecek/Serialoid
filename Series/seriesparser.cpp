#include "series.h"

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
        if (!epOrder.isSet())       // occures when doesnt have episode of season, probably wrong episode at site
            continue;

        Episode* episode = GetOrAddEpisode(epOrder);

        // episode name
        episode->SetName(GetSimpleContent(arr, "a href=\""));

        // episode info
        QString info = GetSimpleContent(arr, "p class=\"synopsis\"");
        if (!info.startsWith("No synopsis available.", Qt::CaseInsensitive))
            episode->SetInfo(info);

        // episode air date
        QStringList dateArr = QString(sub.mid(sub.indexOf(Constant::air)+Constant::air.size()+1, -1)).split("/");
        episode->SetAir(Timestamp(dateArr.value(1)+"."+dateArr.value(0)+"."+dateArr.value(2), GetDailyAir().getTime(), GetDailyAir().getGMT()));

        // episode rating
        episode->SetRating(GetSimpleContent(content, "span class=\"number\"", idx, false).toFloat());
    }
}

void Series::ParseFromIMDB(const QByteArray& content)
{

}

