#ifndef EPISODEORDER_H
#define EPISODEORDER_H

#include <QStringList>

struct EpisodeOrder
{
    uint season;
    uint episode;

    EpisodeOrder(QString str) : season(0), episode(0)
    {
        // for format example used: Season 4 Episode 11
        str = str.toLower();
        switch(str.size())
        {
            case 3:
            {
                // format 411
                if (str[0].isDigit() && str[1].isDigit() && str[2].isDigit())
                {
                    season = str.left(1).toUInt();
                    episode = str.mid(1,2).toUInt();
                    break;
                }
                break;
            }
            case 4:
            {
                // format 0411
                if (str[0].isDigit() && str[1].isDigit() && str[2].isDigit() && str[3].isDigit())
                {
                    season = str.left(2).toUInt();
                    episode = str.mid(2,2).toUInt();
                    break;
                }
                // format 4x11
                if (str[0].isDigit() && QString(str[1]) == "x" && str[2].isDigit() && str[3].isDigit())
                {
                    season = str.left(1).toUInt();
                    episode = str.mid(2,2).toUInt();
                    break;
                }
                break;
            }
            case 5:
            {
                // format 04x11
                if (str[0].isDigit() && str[1].isDigit() && QString(str[2]) == "x" && str[3].isDigit() && str[4].isDigit())
                {
                    season = str.left(2).toUInt();
                    episode = str.mid(3,2).toUInt();
                    break;
                }
                // format s4e11
                if (QString(str[0]) == "s" && str[1].isDigit() && QString(str[2]) == "e" && str[3].isDigit() && str[4].isDigit())
                {
                    season = str.mid(1,1).toUInt();
                    episode = str.mid(3,2).toUInt();
                    break;
                }
                break;
            }
            case 6:
            {
                // format s04e11
                if (QString(str[0]) == "s" && str[1].isDigit() && str[2].isDigit() &&
                    QString(str[3]) == "e" && str[4].isDigit() && str[5].isDigit())
                {
                    season = str.mid(1,2).toUInt();
                    episode = str.mid(4,2).toUInt();
                    break;
                }
                break;
            }
            default:
                break;
        }
    }

    EpisodeOrder(uint _season = 0, uint _episode = 0) : season(_season), episode(_episode) {}

    QString GetNormalLook() const
    {
        QString s = season < 10 ? "0"+QString::number(season) : QString::number(season);
        QString e = episode < 10 ? "0"+QString::number(episode) : QString::number(episode);
        return "S"+s+"E"+e;
    }


    bool isSet() const { return season != 0 && episode != 0; }
};

#endif // EPISODEORDER_H
