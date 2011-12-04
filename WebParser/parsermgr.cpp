#include "parsermgr.h"

ParserMgr::ParserMgr()
{
}

ParserMgr::~ParserMgr()
{

}

QUrl ParserMgr::GetUrl()
{
    QString url = "http://www.imdb.com/title/tt";//+QString::number(imdbId_m);
    if (true)
        url += "/episodes";

    return QUrl(url);
}
