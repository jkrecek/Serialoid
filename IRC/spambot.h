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

#define MUTE_TIME 15

class Bot : public QObject
{
    Q_OBJECT

    public:
        Bot(QObject* parent = 0);

        void Kick(User* user, QString reason);
        void GenerateNewKey();

        QList<User*> Users;

    protected:
        void HandleTimeComparison(const Message& message);
        void HandleSeriesCommands(const Message& message);
        void HandleProfileCommands(const Message& message);
        void HandleHashCommand(const Message& message);

        bool CanAccessSeriesCommands(const Message& message);

    private slots:
        void handleReceivedMessage(const Message& message);

    private:
        IRCServer*      server_m;
        ParserMgr*      parser_m;

        CommandParser*  command;
        ChannelMuteMap  muteMap_m;
};

#endif
