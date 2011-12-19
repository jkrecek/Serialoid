#include "series.h"

typedef QMap<int, int> IndexMap;

void Series::ParseFromTV(const QByteArray& content)
{
    static Site site = SITE_TV;
    // get title
    QString titleTitle = GetTitleFromHead(content,site);
    if (!seriesTitles_m.contains(titleTitle))
        seriesTitles_m.push_front(titleTitle);

    IndexMap m_map;
    int idx = 0;
    QString seasonContent = GetAdvancedContent(content, "h3 class=\"season_name toggle\"", idx);
    do
    {
        seasonContent.remove("Season", Qt::CaseInsensitive);
        m_map.insert(idx, seasonContent.trimmed().toInt());
        seasonContent = GetAdvancedContent(content, "h3 class=\"season_name toggle\"", idx);
    }
    while(idx != -1);

    IndexMap::Iterator curr_season = m_map.begin();
    idx = 0;
    while (idx != -1)
    {
        if (idx > (curr_season+1).key())
            ++curr_season;

        QByteArray arr = GetAdvancedContent(content, "li class=\"episode expanded _clearfix\"", idx);
        if (arr.isEmpty())
            break;

        QString ep = GetSimpleContent(arr, "div class=\"toggle\"", 0);
        ep.remove("Ep", Qt::CaseInsensitive);
        EpisodeOrder epOrder(curr_season.value(), ep.trimmed().toInt());
        if (!epOrder.isSet())       // occures when doesnt have episode of season, probably wrong episode at site
            continue;

        Episode* episode = GetOrAddEpisode(epOrder);
        // episode name
        episode->SetName(GetSimpleContent(arr, "a class=\"title\" href=\""));

        // episode info
        QString info = GetSimpleContent(arr, "div class=\"description\"");
        RemoveAllHtmlTags(info);
        if (!info.startsWith("No synopsis available.", Qt::CaseInsensitive))
            episode->SetInfo(info);

        // episode air date
        QStringList dateArr = QString(GetSimpleContent(arr, "div class=\"date\"")).split("/");
        int year = dateArr.value(2).toInt();
        if (year < 1970)
            year += 2000;
        episode->SetAir(Timestamp(dateArr.value(1)+"."+dateArr.value(0)+"."+QString::number(year), GetDailyAir().getTime(), GetDailyAir().getGMT()));

        // episode rating
        episode->SetRating(GetSimpleContent(arr, "div class=\"_rating rated_8\"", 0).toFloat());
    }
}

void Series::ParseFromIMDB(const QByteArray& content)
{

}

