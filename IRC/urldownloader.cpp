#include "urldownloader.h"

UrlDownloader::UrlDownloader()
{
    manager_m = new QNetworkAccessManager();
    connect(manager_m, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

UrlDownloader::~UrlDownloader()
{
    manager_m->deleteLater();
}

void UrlDownloader::Download(QUrl url)
{
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    request.setRawHeader("User-Agent", "User-Agent:Mozilla/5.0 (Windows NT 6.1) AppleWebKit/535.2 (KHTML, like Gecko) Chrome/15.0.874.106 Safari/535.2");
    manager_m->get(request);
}

void UrlDownloader::replyFinished(QNetworkReply * rep)
{
    emit downloadComplete(rep->url(), rep->readAll());
    rep->deleteLater();
}
