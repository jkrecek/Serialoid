#include <ctime>
#include <iostream>
#include <math.h>
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
    server_m->connectAs("Serialoid", "BOT", "BOT", "Kurva_tahnite_mi_z_nicku");

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
                else
                    server_m->sendMessageToChannel(message.senderChannel(), "No more episodes to be aired for series "+series->GetMainTitle()+"!");
            }
            else if (commands[2] == INFO)
            {
                if (!series->GetInfo().isEmpty())
                {
                    server_m->sendMessageToChannel(message.senderChannel(), ".:"+series->GetMainTitle()+" - info:.");
                    server_m->sendMessageToChannel(message.senderChannel(), series->GetInfo());
                }
                else
                    server_m->sendMessageToChannel(message.senderChannel(), "No info for series "+series->GetMainTitle()+" found!");
            }

            else if (commands[2] == TITLES)
            {
                QString known = series->GetTitles().join(", ");
                if (!known.isEmpty())
                    server_m->sendMessageToChannel(message.senderChannel(), "Known titles for series with codename '"+series->GetName()+"': "+known);
                else
                    server_m->sendMessageToChannel(message.senderChannel(), "Series is known only by its codename '"+series->GetName()+"'");
            }
            else
            {
                EpisodeOrder epOrder(commands[2]);
                if (!epOrder.isSet())
                    return;

                if (Episode* episode = series->GetEpisodeByOrder(epOrder))
                {
                    server_m->sendMessageToChannel(message.senderChannel(), episode->GetAirString());
                    if (commands.size() == 4 && commands[3] == "info")
                    {
                        server_m->sendMessageToUser(message.senderNick(), "---!!! SPOILER ALERT !!!---");
                        server_m->sendMessageToUser(message.senderNick(), episode->GetInfo());
                        server_m->sendMessageToUser(message.senderNick(), "---!!! SPOILER ALERT !!!---");
                    }
                }
                else
                    server_m->sendMessageToChannel(message.senderChannel(), "Episode "+epOrder.GetNormalLook()+" not found!");
            }
        }
    }
    else if (commands[0] == PROFILE)
    {
        if (commands[1] == "add")
        {
            if (commands.size() > 3)
            {
                QString& profileName = commands[2];
                if (profileName.isEmpty())
                {
                    server_m->sendMessageToChannel(message.senderChannel(), "Profile name cannot be empty!");
                    return;
                }
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
            if (commands.size() < 3)
                return;

            if (commands[2] == "next")
            {
                if (commands.size() != 3)
                    return;

                foreach(Series* series, profile->GetProfileSeries())
                    if (Episode* ep = series->GetNextEpisode())
                        server_m->sendMessageToChannel(message.senderChannel(), series->GetMainTitle()+": "+ep->GetAirString());
            }
            else if (commands[2] == "pass")
            {
                if (commands.size() == 4)
                {
                    if (!profile->GetPassHash().isEmpty())
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), "Password already set, if you wish to change it please type 'profile "+profile->GetName()+" pass old new'");
                        return;
                    }

                    profile->SetPassHash(isSha1Hash(commands[3]) ? commands[3] : getHashFor(commands[3]));
                    server_m->sendMessageToChannel(message.senderChannel(), "Password succesfully set!");
                }
                else if (commands.size() == 5)
                {
                    if (profile->GetPassHash().isEmpty())
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), "Cannot change password when none is set!");
                        return;
                    }

                    if (!profile->IsPassCorrect(commands[3]))
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), "Wrong password!");
                        return;
                    }

                    profile->SetPassHash(isSha1Hash(commands[4]) ? commands[4] : getHashFor(commands[4]));
                    server_m->sendMessageToChannel(message.senderChannel(), "Password succesfully edited!");
                }
                else
                    return;

                profileMgr->Save();
            }
            else if (commands[2] == "edit")
            {
                if (commands.size() < 5)
                    return;

                if (!profile->IsPassCorrect(commands[3]))
                {
                    server_m->sendMessageToChannel(message.senderChannel(), "Wrong password!");
                    return;
                }

                profile->SetSeries(message.content().mid(message.content().indexOf(commands[4])), lSeries_m);

                QString s;
                foreach(Series* series, profile->GetProfileSeries())
                {
                    if (!s.isEmpty())
                        s.append(", ");

                    s.append(series->GetMainTitle()+"("+series->GetName()+")");
                }
                server_m->sendMessageToChannel(message.senderChannel(), "Succesfully edited profile '"+profile->GetName()+"' containing series: "+s);
            }
        }
    }
    else if (commands[0] == "hash")
    {
        if (commands.size() != 2)
            return;

        server_m->sendMessageToChannel(message.senderChannel(), getHashFor(commands[1]));
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
    if (qStrList.size() != 3)
        return;

    Timestamp stamp(qStrList[1], qStrList[2]);
    server_m->sendMessageToChannel(channel, "Unix time for timestamp "+stamp.write(FORMAT_TIME_DATE)+" is "+stamp.write(FORMAT_UNIX)+" at actual time ");
    server_m->sendMessageToChannel(channel, "Current timestamp is UNIX:"+Timestamp().write(FORMAT_UNIX)+" STAMP:"+Timestamp().write(FORMAT_TIME_DATE));
}
