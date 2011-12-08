#ifndef EPISODE_H
#define EPISODE_H

#include <QStringList>
#include "episodeorder.h"
#include "timestamp.h"

class Episode
{
    public:
        Episode(EpisodeOrder _order) : Order_m(_order) {}

        void SetName(QString str)           { Name_m = str; }
        void SetInfo(QString str)           { Info_m = str; }
        void SetAir(Timestamp air)          { Air_m = air; }

        EpisodeOrder GetOrder() const       { return Order_m; }
        QString GetName() const             { return Name_m; }
        QString GetInfo() const             { return Info_m; }
        Timestamp GetAir() const            { return Air_m; }

        QString GetOrderString() const      { return Order_m.GetNormalLook(); }
        QString GetAirString() const
        {
            QString air = Air_m.passed() ? "was aired" : "will be aired in "+Air_m.getTo();
            return GetOrderString()+" \""+Name_m+"\" "+air+" at "+Air_m.write(FORMAT_DAY_S_DATE_TIME_GMT);
        }

    private:
        EpisodeOrder Order_m;
        QString Name_m;
        QString Info_m;
        Timestamp Air_m;
};

#endif // EPISODE_H
