#ifndef PARSERMGR_H
#define PARSERMGR_H

#include <QObject>
#include <QUrl>

enum Sites
{
    SITE_IMDB       = 0,
    SITE_TV         = 1
};

#define SITE_COUNT 2

enum Details
{
    DETAILS_DEFAULT = 0,
    DETAILS_EPISODES= 1,
    DETAILS_RATING  = 2
};

#define DETAILS_COUNT 3

class ParserMgr : public QObject
{
    Q_OBJECT

    public:
        ParserMgr();
        ~ParserMgr();

        QUrl GetUrl();
    signals:

    public slots:

    private:


};

#endif // PARSERMGR_H
