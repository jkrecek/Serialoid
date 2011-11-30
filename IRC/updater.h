#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QTimer>
#include "spambot.h"

class Bot;

class Updater : public QObject
{
    Q_OBJECT

    public:
        Updater(Bot *main);
        ~Updater();

    private slots:
        void update();

    private:
        Bot* main_m;
        QTimer* updateTimer_m;
        uint lastUpdate_m;
};

#endif // UPDATER_H
