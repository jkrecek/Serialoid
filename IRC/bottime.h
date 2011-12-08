#ifndef TIME_H
#define TIME_H

#include <QObject>
#include <QTimer>
#include <QTime>

class BotTime : public QObject
{
    Q_OBJECT

    public:
        BotTime();
        ~BotTime();

        int getMSTime() const { return time_m->elapsed(); }

    signals:
        void Update(const int diff);

    private slots:
        void update();

    private:
        QTime*  time_m;
        QTimer* timer_m;
        int     lastUpdate_m;
};

#define sBotTime Singleton<BotTime>::Instance()

#endif // TIME_H
