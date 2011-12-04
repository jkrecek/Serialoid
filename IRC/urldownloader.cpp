#include <QDebug>
#include "urldownloader.h"

UrlDownloader::UrlDownloader()
{
    manager_m = new QNetworkAccessManager();
    connect(manager_m, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

UrlDownloader::~UrlDownloader()
{
    //delete manager_m;
}

void UrlDownloader::Get(QString url)
{
    QNetworkRequest request(QUrl(url));
    request.setRawHeader("User-Agent", "User-Agent:Mozilla/5.0 (Windows NT 6.1) AppleWebKit/535.2 (KHTML, like Gecko) Chrome/15.0.874.106 Safari/535.2");
    manager_m->get(request);
}

void UrlDownloader::replyFinished(QNetworkReply * rep)
{
    emit recievedData(rep->readAll());
}
