#include <ctime>
#include <iostream>
#include <math.h>
#include <QDebug>
#include <QFile>
#include <QStringList>
#include "episode.h"
#include "episodeorder.h"
#include "Formulas.h"
#include "channel.h"
#include "ircconstants.h"
#include "ircserver.h"
#include "seriesparser.h"
#include "spambot.h"
#include "user.h"

#define ROUTE_SERIES_FILE "C://series.txt"
#define ROUTE_PROFILE_FILE "C://profile.txt"
#define ROUTE_ERROR_FILE "C://error.txt"

Bot::Bot(QObject* parent) : QObject(parent)
{
    SeriesParser(ROUTE_SERIES_FILE, ROUTE_ERROR_FILE, lSeries_m);
    profileMgr = new ProfileMgr(ROUTE_PROFILE_FILE, ROUTE_ERROR_FILE, lSeries_m);

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
        HandleTimeComparison(message.senderChannel(), commands);
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
            server_m->sendMessageToChannel(message.senderChannel(), ".:Printing all known series ( format Name(codename) ):.");
            server_m->sendMessageToChannel(message.senderChannel(), known);
            return;
        }
        else if (Series* series = GetSeries(commands[1]))
        {
            if (commands[2] == NEXT)
            {
                if (Episode* nextEp = series->GetNextEpisode())
                {
                    server_m->sendMessageToChannel(message.senderChannel(), ".:"+series->GetMainTitle()+" - next episode:.");
                    server_m->sendMessageToChannel(message.senderChannel(), nextEp->GetAirString());
                }
            }
            else if (commands[2] == INFO)
            {
                server_m->sendMessageToChannel(message.senderChannel(), ".:"+series->GetMainTitle()+" - info:.");
                server_m->sendMessageToChannel(message.senderChannel(), series->GetInfo());
                return;
            }

            else if (commands[2] == TITLES)
            {
                QString known = series->GetTitles().join(", ");
                server_m->sendMessageToChannel(message.senderChannel(), "Known titles for series with codename '"+series->GetName()+"': "+known);
            }
            else if (Episode* episode = series->GetEpisodeByOrder(commands[2]))
            {
                server_m->sendMessageToChannel(message.senderChannel(), episode->GetAirString());
                if (commands.size() == 4 && commands[3] == "info")
                {
                    server_m->sendMessageToUser(message.senderNick(), "---!!! SPOILER ALERT !!!---");
                    server_m->sendMessageToUser(message.senderNick(), episode->GetInfo());
                    server_m->sendMessageToUser(message.senderNick(), "---!!! SPOILER ALERT !!!---");
                }
            }
        }
    }
    else if (commands[0] == PROFILE)
    {
        // profile add spammca himym
        if (commands[1] == "add")
        {
            if (commands.size() > 3)
            {
                QString& profileName = commands[2];
                if (profileMgr->isNameForbidden(profileName))
                {
                    server_m->sendMessageToChannel(message.senderChannel(), "You cannot use profile name "+profileName+". Such name is forbidden!");
                    return;
                }
                Profile* profile = profileMgr->GetProfile(profileName);
                if (!profile)
                {
                    profile = new Profile(profileName);
                    profile->SetSeries(message.content().mid(message.content().indexOf(commands[3])), lSeries_m);
                    profileMgr->AddProfile(profile);

                    QString s;
                    foreach(Series* series, profile->GetProfileSeries())
                    {
                        if (!s.isEmpty())
                            s.append(", ");

                        s.append(series->GetMainTitle()+"("+series->GetName()+")");
                    }
                    server_m->sendMessageToChannel(message.senderChannel(), "Succesfully added profile '"+profileName+"' containing series: "+s);
                }
                else
                    server_m->sendMessageToChannel(message.senderChannel(), "Profile already exists!");
            }
        }
        else if (Profile* profile = profileMgr->GetProfile(commands[1]))
        {
            if (commands[2] == "next")
            {
                foreach(Series* series, profile->GetProfileSeries())
                    if (Episode* ep = series->GetNextEpisode())
                        server_m->sendMessageToChannel(message.senderChannel(), series->GetMainTitle()+": "+ep->GetAirString());
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

void Bot::HandleTimeComparison(QString channel, QStringList qStrList)
{
    Timestamp stamp(qStrList[1], qStrList[2]);
    server_m->sendMessageToChannel(channel, "Unix time for timestamp "+stamp.write(FORMAT_TIME_DATE)+" is "+stamp.write(FORMAT_UNIX)+" at actual time ");
    server_m->sendMessageToChannel(channel, "Current timestamp is UNIX:"+Timestamp().write(FORMAT_UNIX)+" STAMP:"+Timestamp().write(FORMAT_TIME_DATE));
}
