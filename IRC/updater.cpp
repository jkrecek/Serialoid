#include "updater.h"
#include <QDebug>
#include <time.h>

Updater::Updater(Bot *main) : QObject(NULL), main_m(main)
{
    updateTimer_m = new QTimer(this);
    lastUpdate_m = uint(time(0));
    connect(updateTimer_m, SIGNAL(timeout()), this, SLOT(update()));
    updateTimer_m->start(5000);
}

Updater::~Updater()
{
    delete updateTimer_m;
}

void Updater::update()
{
    const uint now = uint(time(0));
    const uint diff = now - lastUpdate_m;
    main_m->Update(diff);
    lastUpdate_m = now;
}
