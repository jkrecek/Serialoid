#ifndef SPAMBOT_H
#define SPAMBOT_H

#include <QMap>
#include <QObject>
#include <QStringList>
#include "message.h"
#include "profilemgr.h"
#include "series.h"
#include "user.h"

class IRCServer;
class ProfileMgr;

typedef QMap<QString, Series*> SeriesMap;
typedef QMap<QString, uint> ChannelMuteMap;

#define MUTE_TIME 15

class Bot : public QObject
{
    Q_OBJECT

    public:
        Bot(QObject* parent = 0);

        void Kick(User* user, QString reason);
        void GenerateNewKey();
        Series* GetSeries(QString name) const { return lSeries_m.contains(name) ? lSeries_m[name] : NULL; }

        QList<User*> Users;

    private slots:
        void handleReceivedMessage(const Message& message);

    private:
        void HandleTimeComparison(const Message& message);
        void HandleSeriesCommands(const Message& message);
        void HandleProfileCommands(const Message& message);
        void HandleHashCommand(const Message& message);

        bool CanAccessSeriesCommands(const Message& message);
        IRCServer* server_m;
        ProfileMgr* profileMgr;

        SeriesMap lSeries_m;
        ChannelMuteMap muteMap_m;
};
#endif
