#ifndef EPISODE_H
#define EPISODE_H

#include <QStringList>
#include "episodeorder.h"
#include "timestamp.h"

class Episode
{
    public:
        Episode(uint Id) : Id_m(Id) {}

        Timestamp GetAir() const { return air_m; }
        QString GetName() const { return Name_m; }
        QString GetEpisodeOrder() const { return Order_m.GetNormalLook(); }
        QString GetInfo() const { return Info_m; }
        uint GetId() const { return Id_m; }

        void SetName(QString str) { Name_m = str; }
        void SetInfo(QString str) { Info_m = str; }
        void SetAir(Timestamp air) { air_m = air; }
        void SetEpisodeOrder(EpisodeOrder order) { Order_m = order; }
    private:
        uint Id_m;
        EpisodeOrder Order_m;
        QString Name_m;
        QString Info_m;
        Timestamp air_m;
};

#endif // EPISODE_H
