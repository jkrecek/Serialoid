#ifndef USER_H
#define USER_H

#include <QString>
#include "message.h"
#include <QStringList>
#include <QList>
#include <QMap>

class IRCServer;

class User
{
    public:
        User(IRCServer* server_m);

        IRCServer* server() const { return server_m; }

    private:
        IRCServer* server_m;

};

#endif // USER_H
