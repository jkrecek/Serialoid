#include <QDebug>
#include "urldownloader.h"

UrlDownloader::UrlDownloader()
{
    manager_m = new QNetworkAccessManager(this);
    connect(manager_m, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

UrlDownloader::~UrlDownloader()
{
    //delete manager_m;
}

void UrlDownloader::Get(QString url)
{
    manager_m->get(QNetworkRequest(QUrl(url)));
}

void UrlDownloader::replyFinished(QNetworkReply * rep)
{
    emit recievedData(rep->readAll());
}
