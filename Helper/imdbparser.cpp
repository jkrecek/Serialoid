#include <QDebug>
#include "imdbparser.h"
#include "urldownloader.h"
#include "profilemgr.h"

ImdbParser::ImdbParser(uint _imdbId) : QObject(NULL), imdbId_m(_imdbId)
{
    //sUDownloader.Get(GetUrl(false).toString());
    //connect(&sUDownloader, SIGNAL(recievedData(QByteArray)), this, SLOT(parseAll(QByteArray)));
}

ImdbParser::~ImdbParser()
{
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


