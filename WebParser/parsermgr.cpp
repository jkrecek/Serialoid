#include "parsermgr.h"
#include "seriesmgr.h"
#include "urldownloader.h"
#include "tvparser.h"
#include <QDebug>

ParserMgr::ParserMgr()
{
    // this should not be needed
    currentItr_m = NULL;
    parseMap_m.clear();

    // fill parse map
    foreach(Series* series, sSeries.GetMap())
        foreach(const QString& link, series->GetLinks())
            parseMap_m.insert(link, series->GetName());

    // connect do downloader
    connect(&sUDownloader, SIGNAL(downloadComplete(QUrl, QByteArray)), this, SLOT(handleRecieved(QUrl,QByteArray)));

    // start downloading
    currentItr_m = parseMap_m.begin();
    sUDownloader.Download(currentItr_m.key());
}

Site ParserMgr::GetSiteFromLink(QString link)
{
    // remove prefix of link to get pure site string
    link.remove("http://", Qt::CaseInsensitive);
    link.remove("www.", Qt::CaseInsensitive);
    QString siteString = link.left(link.indexOf("."));
    return sSeries.GetSiteFromString(siteString);
}

void ParserMgr::handleRecieved(QUrl url, const QByteArray &content)
{
    QString oldLink = currentItr_m.key();
    if (url.toString() != oldLink)
    {
        qDebug() << "Something went terribly wrong, recieved another url then expected!";
        return;
    }

    Series* currSeries = sSeries.GetSeries(currentItr_m.value());

    // all needed values from iterator saved, now we can erase it
    parseMap_m.erase(currentItr_m);

    if (!parseMap_m.empty())
    {
        // if map is not empty find now iterator ..
        currentItr_m = parseMap_m.begin();
        // .. and start downloading
        sUDownloader.Download(currentItr_m.key());
    }

    switch(GetSiteFromLink(oldLink))
    {
        case SITE_TV:
            TVParser(currSeries, content);
            break;
        case SITE_IMDB:
        default:
            break;
    }

    if (parseMap_m.empty())
    {
        currentItr_m = NULL;
        // if completed let know Bot class
        emit allParsed();
    }
}
