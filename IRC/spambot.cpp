#include "spambot.h"
#include "ircserver.h"
#include "user.h"
#include "channel.h"
#include "ircconstants.h"
#include "spambot.h"
#include "Formulas.h"
#include "episode.h"
#include "episodeorder.h"
#include "setting.h"

#include <QStringList>
#include <QDebug>
#include <QFile>
#include <iostream>
#include <ctime>
#include <math.h>

Bot::Bot(QObject* parent) : QObject(parent)
{
    Setting setting("C://settings.txt");
    lSeries_m = setting.lSeries_m;

    // connecting to servers
    server_m = new IRCServer("irc.rizon.net", 6667);
    server_m->connectAs("Serijaloid", "BOT", "BOT", "Kurva_tahnite_mi_z_nicku");

    server_m->joinChannel("#valhalla");

    qsrand(sqrt(time(0))*2);

    connect(server_m, SIGNAL(messageReceived(Message)), this, SLOT(handleReceivedMessage(Message)));
}

void Bot::handleReceivedMessage(const Message& message)
{
    // do not act where is off or is sended by bot itself
    //if (message.senderNick() == server_m->ownNick())
    //   return;

    QStringList commands = message.content().split(" ");
    // rejoin on kick (TODO:: do not rejoin on every kick)
    if (message.command() == IRC::Command::Kick)
        server_m->joinChannel(message.senderChannel());

    if (commands[0] == COMPARE)
    {
        HandleTimeComparison(commands);
        return;
    }
    else if (commands[0] == SERIES)
    {
        if (commands[1] == LIST)
        {
            QString known;
            foreach(Series* series, lSeries_m)
            {
                if (!known.isEmpty())
                    known.append(", ");

                known.append(series->GetMainTitle()+" ("+series->GetName()+")");
            }
            server_m->sendMessageToChannel("#valhalla", ".:Printing all known series ( format Name(codename) ):.");
            server_m->sendMessageToChannel("#valhalla", known);
            return;
        }
        else if (Series* series = GetSeries(commands[1]))
        {
            if (commands[2] == NEXT)
            {
                if (Season* season = series->GetCurrentSeason())
                {
                    EpisodeMap& epM = season->GetEpisodes();
                    server_m->sendMessageToChannel("#valhalla", ".:"+series->GetMainTitle()+" - next episode:.");
                    foreach(Episode* episode, epM)
                    {
                        if (episode && !episode->GetAir().passed())
                        {
                            server_m->sendMessageToChannel("#valhalla", ""+episode->GetEpisodeOrder()+" \""+episode->GetName()+"\" will be aired in "+episode->GetAir().getTo()+" (airs at "+episode->GetAir().write(FORMAT_DAY_S_TIME_DATE)+" "+episode->GetAir().writeGMT()+")");
                            break;
                        }
                    }
                    return;
                }
            }
            else if (commands[2] == INFO)
            {
                server_m->sendMessageToChannel("#valhalla", ".:"+series->GetMainTitle()+" - info:.");
                server_m->sendMessageToChannel("#valhalla", series->GetInfo());
                return;
            }

            else if (commands[2] == TITLES)
            {
                QString known;
                foreach(QString title, series->GetTitles())
                {
                    if (!known.isEmpty())
                        known.append(", ");

                    known.append(title.trimmed());
                }
                server_m->sendMessageToChannel("#valhalla", "Known titles for series with codename '"+series->GetName()+"': "+known);
            }
            else if (Episode* episode = series->GetEpisodeByOrder(commands[2]))
            {
                server_m->sendMessageToChannel("#valhalla", "Episode "+episode->GetEpisodeOrder()+" \""+episode->GetName()+"\" "+QString(episode->GetAir().passed() ? "was" : "will be")+" aired at "+episode->GetAir().write(FORMAT_DAY_S_TIME_DATE)+" "/*+GMTLook(episode->GetHourDiff())*/);
                if (commands.size() == 4 && commands[3] == "info")
                {
                    server_m->sendMessageToChannel("#valhalla", "---!!! SPOILER ALERT !!!---");
                    server_m->sendMessageToChannel("#valhalla", episode->GetInfo());
                    server_m->sendMessageToChannel("#valhalla", "---!!! SPOILER ALERT !!!---");
                }
            }
        }
    }

    if (message.senderNick() == server_m->ownNick() && message.content() == "quit")
        exit(1);

    if (!message.isServerMessage() && message.command() == IRC::Command::PrivMsg)
    {
        if (message.content().toLower().contains(server_m->ownNick().toLower()))
            server_m->sendMessageToChannel(message.senderChannel(), "Coe?!?!?!");

    }
}

void Bot::HandleTimeComparison(QStringList qStrList)
{
    Timestamp stamp(qStrList[1], qStrList[2]);
    server_m->sendMessageToChannel("#valhalla", "Unix time for timestamp "+stamp.write(FORMAT_TIME_DATE)+" is "+stamp.write(FORMAT_UNIX)+" at actual time ");
    server_m->sendMessageToChannel("#valhalla", "Current timestamp is UNIX:"+Timestamp().write(FORMAT_UNIX)+" STAMP:"+Timestamp().write(FORMAT_TIME_DATE));
}
