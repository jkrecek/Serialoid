#ifndef SPAMBOT_H
#define SPAMBOT_H

#include <QMap>
#include <QObject>
#include <QStringList>
#include <QTime>
#include "commandparser.h"
#include "message.h"
#include "profilemgr.h"
#include "series.h"
#include "user.h"
#include "parsermgr.h"
#include "updater.h"


class IRCServer;
class ProfileMgr;
class CommandParser;
class Updater;

typedef QMap<QString, uint> ChannelMuteMap;

#define MUTE_TIME 15

class Bot : public QObject
{
    Q_OBJECT

    public:
        Bot(QObject* parent = 0);

        void Kick(User* user, QString reason);
        void GenerateNewKey();

        QList<User*> Users;
        void Update(const uint diff);

    protected:
        void HandleTimeComparison(const Message& message);
        void HandleSeriesCommands(const Message& message);
        void HandleProfileCommands(const Message& message);
        void HandleHashCommand(const Message& message);

        uint GetCooldownEndTime(const Message& message);

    private slots:
        void handleReceivedMessage(const Message& message);
        void parsingComplete();

    private:
        IRCServer*      server_m;
        ParserMgr*      parser_m;
        Updater*        updater_m;

        CommandParser*  command_m;
        ChannelMuteMap  muteMap_m;
};

#define sTime Singleton<QTime>::Instance()

#endif
