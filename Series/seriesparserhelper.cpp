#include "series.h"

QByteArray Series::GetSimpleContent(const QByteArray &content, QByteArray fullTag, int start, bool forward)
{
    int itsStart;
    if (forward)
        itsStart = content.indexOf("<"+fullTag, start);         // index of opening tag begin, default look forward
    else
        itsStart = content.lastIndexOf("<"+fullTag, start);     // index of opening tag begin, look backwards

    if (itsStart == -1)                                         // start tag not found while searching from start point
        return QByteArray();

    int itsEnd = content.indexOf(">", itsStart)+1;              // index of opening tag end
    QByteArray simpleTag = (fullTag.contains(" ") ? fullTag.left(fullTag.indexOf(" ")) : fullTag);
    int iteStart = content.indexOf("</"+simpleTag+">", itsEnd); // index of closing tag start
    int length = iteStart - itsEnd;                             // difference between itsEnd and iteStart, aka length of desired string
    if (length < -1)                                            // should not never happend
        length = -1;

    return content.mid(itsEnd, length).trimmed();
}

//QByteArray Series::GetAllTags(const QByteArray &content, QByteArray tag)
//{
//    QMap<int,QString> m_map;
//    int from = 0;
//    // find all starting tags
//    int itsStart;
//    while(from != -1)
//    {
//        itsStart = content.indexOf("<"+tag, from);
//        if (itsStart == -1)
//            break;
//        m_map.insert(itsStart, content.mid(itsStart, content.indexOf(">", itsStart)-itsStart+1));
//        from = itsStart+1;
//    }
//    // find all ending tags
//    from = 0;
//    while(from != -1)
//    {
//        itsStart = content.indexOf("</"+tag+">", from);
//        if (itsStart == -1)
//            break;
//        m_map.insert(itsStart, "</"+tag+">");
//        from = itsStart + 1;
//    }
//
//    for(QMap<int,QString>::Iterator itr = m_map.begin(); itr != m_map.end(); ++itr)
//        qDebug << "Idx: '"+QString::number(itr.key())+"'' Value: '"+itr.value()+"'";
//
//    return QByteArray();
//}

QByteArray Series::GetAdvancedContent(const QByteArray &content, QByteArray fullTag, int& idx)
{
    idx = content.indexOf("<"+fullTag, idx);
    if (idx == -1)
        return QByteArray();

    QByteArray simpleTag = fullTag.left(fullTag.indexOf(" "));
    int sEndIdx = content.indexOf(">", idx)+1;
    uint currentOpen = 1;
    while(currentOpen && idx != -1)
    {
        idx += simpleTag.size(); // increase index so we shouldnt found same tag

        idx = content.indexOf(simpleTag, idx);
        if (content.at(idx-1) == Constant::tagStart)
            ++currentOpen;
        else if (content.at(idx-2) == Constant::tagStart && content.at(idx-1) == Constant::slash)
            --currentOpen;
    }

    if (currentOpen)
        return QByteArray();

    return content.mid(sEndIdx, idx-sEndIdx-2).trimmed();
}

QString Series::GetTitleFromHead(const QByteArray& content, Site site)
{
    // get title byte array and convert it into QString
    QString title = GetSimpleContent(content, "title");

    // now strip it of useless data
    switch(site)
    {
        case SITE_IMDB:
            title.remove("IMDb - ", Qt::CaseInsensitive);
            title.remove(title.indexOf("(", 0, Qt::CaseInsensitive), title.size());
            break;
        case SITE_TV:
            title.remove(" - TV.com", Qt::CaseInsensitive);
            title.remove("Episode Guide", Qt::CaseInsensitive);
            if (title.contains("Season", Qt::CaseInsensitive))
                title = title.left(title.indexOf("Season 1-", Qt::CaseInsensitive)).trimmed();
            break;
        default:
            break;
    }

    return title.trimmed();
}

void Series::RemoveAllHtmlTags(QString &content)
{
    content.remove("<p>");
    content.remove("</p>");
}
