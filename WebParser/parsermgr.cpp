#include "parsermgr.h"
#include "seriesmgr.h"
#include "urldownloader.h"

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
    QUrl url = GetUrl(currentItr_m.key(), DETAILS_EPISODES);
    sUDownloader.Download(url);
}

Site ParserMgr::GetSiteFromLink(QString link)
{
    // remove prefix of link to get pure site string
    link.remove("http://", Qt::CaseInsensitive);
    link.remove("www.", Qt::CaseInsensitive);
    QString siteString = link.left(link.indexOf("."));
    return sSeries.GetSiteFromString(siteString);
}

void ParserMgr::handleRecieved(QUrl /*url*/, const QByteArray &content)
{
    QString oldLink = currentItr_m.key();
    // must be rewriten, oldLink contains basic link, returned url contains specific link to episodes, rating, etc.
    //if (url.toString() != oldLink)
    //{
    //    qDebug() << "Something went terribly wrong, recieved another url then expected! Expected '"+oldLink+"' and recieved '"+url.toString()+"'";
    //    return;
    //}

    Series* currSeries = sSeries.GetSeries(currentItr_m.value());
    // all needed values from iterator saved, now we can erase it
    parseMap_m.erase(currentItr_m);

    if (!parseMap_m.empty())
    {
        // if map is not empty find now iterator ..
        currentItr_m = parseMap_m.begin();
        // .. and start downloading
        QUrl url = GetUrl(currentItr_m.key(), DETAILS_EPISODES);
        sUDownloader.Download(url);
    }

    switch(GetSiteFromLink(oldLink))
    {
        case SITE_TV:
            currSeries->ParseFromTV(content);
            break;
        case SITE_IMDB:
            currSeries->ParseFromIMDB(content);
            break;
        default:
            break;
    }

    PrintOut("Series "+currSeries->GetName()+" successfully parsed!");

    if (parseMap_m.empty())
    {
        currentItr_m = NULL;
        // if completed let know Bot class
        emit allParsed();
    }
}

QUrl ParserMgr::GetUrl(QString baseUrl, Details details)
{
    Site site = GetSiteFromLink(baseUrl);
    QString url = baseUrl;
    switch(details)
    {
        case DETAILS_DEFAULT:           // just return baseUrl
            break;

        case DETAILS_EPISODES:
        {
            if (!url.endsWith("/"))     // if url doesn't ends with slash add it
                url.append("/");

            switch(site)
            {
                case SITE_TV:
                    url.append("season/?season=all");
                    break;
                case SITE_IMDB:
                    url.append("episodes");
                    break;
                default:
                    break;
            }
        }
        default:
            break;
    }

    return QUrl(url);
}
