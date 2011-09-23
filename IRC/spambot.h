#ifndef SPAMBOT_H
#define SPAMBOT_H

#include <QMap>
#include <QObject>
#include <QStringList>
#include "message.h"
#include "series.h"
#include "user.h"

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
        void HandleTimeComparison(QString channel, QStringList qStrList);

        IRCServer* server_m;
        SeriesMap lSeries_m;
};
#endif
