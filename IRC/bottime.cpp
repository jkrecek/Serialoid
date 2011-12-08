#include "bottime.h"

BotTime::BotTime()
{
    time_m = new QTime();
    time_m->start();

    timer_m = new QTimer(this);
    connect(timer_m, SIGNAL(timeout()), this, SLOT(update()));
    timer_m->start(2500);

    lastUpdate_m = time_m->elapsed();
}

BotTime::~BotTime()
{
    timer_m->deleteLater();
}

void BotTime::update()
{
    const int elapsed = time_m->elapsed();
    const int diff = elapsed - lastUpdate_m;
    lastUpdate_m = time_m->elapsed();
    emit Update(diff);
}
