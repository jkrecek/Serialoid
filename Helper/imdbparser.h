#ifndef IMDBPARSER_H
#define IMDBPARSER_H

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include "series.h"

class ImdbParser : public QObject
{
    Q_OBJECT

    public:
        ImdbParser(Series* _series, uint _imdbId);
        ~ImdbParser();

    public slots:
        void replyFinished(QNetworkReply*);

    private:
        void parseAll(const QByteArray& content);
        QUrl GetUrl(bool episode);
        QByteArray GetContentInTag(const QByteArray& content, QByteArray tag, int start = 0);

        QNetworkAccessManager* manager_m;
        Series* series_m;
        uint imdbId_m;
};

#endif // IMDBPARSER_H
