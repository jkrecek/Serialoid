#ifndef CHANNEL_H
#define CHANNEL_H

#include <QMap>
#include <QObject>
#include <QString>
#include "message.h"

class IRCServer;
class User;

class Channel : public QObject
{
    Q_OBJECT

    public:
        Channel(const QString& channelName, IRCServer* server);

        QList<User*> channelUsers();
        QList<QString> channelUsersNicks();

    private slots:
        void handleMessage(const Message& message);

    private:
        void handleChannelMessage(const Message& message);
        void addUser(const QString& userName);
        void removeUser(const QString& userName);
        static QString purifyUserName(const QString& userName);

        QString channelName_m;
        IRCServer* server_m;
        QMap<QString, User*> channelUsers_m;
};

#endif // CHANNEL_H
