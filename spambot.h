#ifndef SPAMBOT_H
#define SPAMBOT_H

#include <QObject>
#include "message.h"
#include "user.h"
#include "series.h"
#include <QStringList>
#include <QMap>

class IRCServer;

typedef QMap<QString, Series*> SeriesMap;

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
        void HandleTimeComparison(QStringList qStrList);

        IRCServer* server_m;
        SeriesMap lSeries_m;
};
#endif
