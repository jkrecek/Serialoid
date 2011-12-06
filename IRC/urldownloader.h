#ifndef URLDOWNLOADER_H
#define URLDOWNLOADER_H

#include "singleton.h"
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>

class UrlDownloader : public QObject
{
    Q_OBJECT

    public:
        UrlDownloader();
        ~UrlDownloader();

        void Download(QUrl url);
        void Download(QString url) { return Download(QUrl(url)); }

    signals:
        //                    url         data
        void downloadComplete(QUrl, const QByteArray&);

    private slots:
        void replyFinished(QNetworkReply*);

    private:
        QNetworkAccessManager* manager_m;
};

#define sUDownloader Singleton<UrlDownloader>::Instance()

#endif // URLDOWNLOADER_H
