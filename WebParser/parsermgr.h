#ifndef PARSERMGR_H
#define PARSERMGR_H

#include <QObject>
#include <QUrl>
#include "series.h"

enum Details
{
    DETAILS_DEFAULT = 0,
    DETAILS_EPISODES= 1,
    DETAILS_RATING  = 2
};

#define DETAILS_COUNT 3

//           link     series-codename
typedef QMap<QString, QString> LinkMap;

class ParserMgr : public QObject
{
    Q_OBJECT

    public:
        ParserMgr();
        ~ParserMgr() {}

        Site GetSiteFromLink(QString link);
        QUrl GetUrl(QString baseUrl, Details details);

    signals:
        void allParsed();

    public slots:
        void handleRecieved(QUrl url, const QByteArray& content);

    private:
        LinkMap parseMap_m;
        LinkMap::Iterator currentItr_m;
};

#endif // PARSERMGR_H
