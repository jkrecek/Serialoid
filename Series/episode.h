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
        void SetRating(float rat)           { rating_m = rat; }

        EpisodeOrder GetOrder() const       { return Order_m; }
        QString GetName() const             { return Name_m; }
        QString GetInfo() const             { return Info_m; }
        Timestamp GetAir() const            { return Air_m; }
        float GetRating() const             { return rating_m; }

        QString GetAirString() const
        {
            if (Air_m.Aired())
                return GetOrderString()+" \""+Name_m+"\" "+"was aired at "+Air_m.write(FORMAT_DAY_S_DATE_TIME_GMT);
            else
                return GetOrderString()+" \""+Name_m+"\" "+"will be aired in "+Air_m.getTo()+" at "+Air_m.write(FORMAT_DAY_S_DATE_TIME_GMT);
        }

        QString GetJustAiredString() const
        {
            return GetOrderString()+" \""+Name_m+"\" was just aired ("+Air_m.write(FORMAT_DAY_S_TIME_DATE_GMT)+")";
        }

    private:
        QString GetOrderString() const      { return Order_m.GetNormalLook(); }

        EpisodeOrder Order_m;
        QString Name_m;
        QString Info_m;
        Timestamp Air_m;
        float rating_m;
};

#endif // EPISODE_H
