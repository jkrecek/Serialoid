#ifndef IMDBPARSER_H
#define IMDBPARSER_H

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include "series.h"
#include "urldownloader.h"

class ImdbParser : public QObject
{
    Q_OBJECT

    public:
        ImdbParser(uint _imdbId);
        ~ImdbParser();

    public slots:
        void parseAll(const QByteArray& content);

    private:
        QByteArray GetContentInTag(const QByteArray& content, QByteArray tag, int start = 0);

        uint imdbId_m;
};

#endif // IMDBPARSER_H
