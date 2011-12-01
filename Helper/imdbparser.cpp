#include <QDebug>
#include "imdbparser.h"

ImdbParser::ImdbParser(Series* _series, uint _imdbId) : QObject(NULL), series_m(_series), imdbId_m(_imdbId)
{
    manager_m = new QNetworkAccessManager(this);
    connect(manager_m, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    manager_m->get(QNetworkRequest(GetUrl(false)));
}

ImdbParser::~ImdbParser()
{
    delete manager_m;
}

void ImdbParser::replyFinished(QNetworkReply * reply)
{
    qDebug() << "AAAAAAAAA CHYTAAAAM";
    parseAll(reply->readAll());

}

void ImdbParser::parseAll(const QByteArray &content)
{
    qDebug() << GetContentInTag(content, "title");

}

QByteArray ImdbParser::GetContentInTag(const QByteArray& content, QByteArray tag, int start)
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

QUrl ImdbParser::GetUrl(bool episode)
{
    QString url = "http://www.imdb.com/title/tt"+QString::number(imdbId_m);
    if (episode)
        url += "/episodes";

    qDebug() << url;

    return QUrl(url);
}
