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
#include "parsermgr.h"

class IRCServer;
class ProfileMgr;
class CommandParser;

typedef QMap<QString, uint> ChannelMuteMap;

#define MUTE_TIME 5

class Bot : public QObject
{
    Q_OBJECT

    public:
        Bot(QObject* parent = 0);

        void Kick(User* user, QString reason);
        void GenerateNewKey();

        QList<User*> Users;

    protected:
        bool HandleTimeComparison(const Message& message);
        bool HandleSeriesCommands(const Message& message);
        bool HandleProfileCommands(const Message& message);
        bool HandleHashCommand(const Message& message);

        uint GetCooldownEndTime(const Message& message);

    private slots:
        void handleReceivedMessage(const Message& message);
        void parsingComplete();
        void UpdateBot(const int diff);

    private:
        IRCServer*      server_m;
        ParserMgr*      parser_m;

        CommandParser*  command_m;
        ChannelMuteMap  muteMap_m;
};

#endif
