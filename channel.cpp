#include "channel.h"

#include <QStringList>
#include "ircserver.h"
#include "ircconstants.h"
#include "user.h"

Channel::Channel(const QString& channelName, IRCServer* server) :
    channelName_m(channelName),
    server_m(server)
{
    connect(server_m, SIGNAL(messageReceived(Message)), this, SLOT(handleMessage(Message)));
}

void Channel::handleMessage(const Message& message)
{
    if (message.command() == IRC::Reply::NAMREPLY)
    {
        QStringList channelUserNames = message.content().split(" ");

        foreach(QString userName, channelUserNames)
            addUser(userName);
    }

    if (message.isChannelMessage())
        handleChannelMessage(message);
}

void Channel::handleChannelMessage(const Message& message)
{
    if (message.command() == IRC::Reply::NAMREPLY)
    {   }
}

void Channel::addUser(const QString& userName)
{
    QString purifiedName = purifyUserName(userName);
    //channelUsers_m[purifiedName] = new User(purifiedName, channelName_m, server_m);
}

QString Channel::purifyUserName(const QString& userName)
{
    QString purifiedName = userName;

    if (purifiedName.startsWith('@') ||
        purifiedName.startsWith('~') ||
        purifiedName.startsWith('+')  )
    {
        purifiedName.remove(0, 1);
    }

    return purifiedName;
}

QList<User*> Channel::channelUsers()
{
    return channelUsers_m.values();
}

QList<QString> Channel::channelUsersNicks()
{
    return channelUsers_m.keys();
}
