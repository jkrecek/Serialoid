#ifndef SPAMBOT_H
#define SPAMBOT_H

#include <QMap>
#include <QObject>
#include <QStringList>
#include "commandparser.h"
#include "message.h"
#include "profilemgr.h"
#include "series.h"
#include "user.h"
#include "updater.h"

class IRCServer;
class ProfileMgr;
class CommandParser;
class Updater;

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
        void Update(const uint diff);

    private slots:
        void handleReceivedMessage(const Message& message);

    private:
        void HandleTimeComparison(const Message& message);
        void HandleSeriesCommands(const Message& message);
        void HandleProfileCommands(const Message& message);
        void HandleHashCommand(const Message& message);

        uint GetCooldownEndTime(const Message& message);

        IRCServer* server_m;
        ProfileMgr* profileMgr;
        CommandParser* command_m;
        Updater* updater_m;

        SeriesMap lSeries_m;
        ChannelMuteMap muteMap_m;
};

#endif
