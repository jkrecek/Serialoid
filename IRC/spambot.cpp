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

#define ROUTE_SERIES_FILE "series.txt"
#define ROUTE_PROFILE_FILE "profile.txt"
#define ROUTE_ERROR_FILE "error.txt"

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
    // rejoin on kick (TODO:: do not rejoin on every kick)
    if (message.command() == IRC::Command::Kick)
        server_m->joinChannel(message.senderChannel());

    if (CanAccessSeriesCommands(message))
    {
        QString firstWord = message.content().left(message.content().indexOf(" ")).trimmed();

        if (firstWord == COMPARE)
            HandleTimeComparison(message);
        else if (firstWord == SERIES)
            HandleSeriesCommands(message);
        else if (firstWord == PROFILE)
            HandleProfileCommands(message);
        else if (firstWord == HASH)
            HandleHashCommand(message);
    }

    if (message.senderNick() == server_m->ownNick() && message.content() == "quit")
        exit(1);

    if (!message.isServerMessage() && message.command() == IRC::Command::PrivMsg)
    {
        if (message.content().toLower().contains(server_m->ownNick().toLower()))
            server_m->sendMessageToChannel(message.senderChannel(), "Coe?!?!?!");
    }
}

void Bot::HandleTimeComparison(const Message &message)
{
    QStringList qStrList = message.content().split(" ");
    if (qStrList.size() != 3)
        return;

    Timestamp stamp(qStrList[1], qStrList[2]);
    server_m->sendMessageToChannel(message.senderChannel(), "Unix time for timestamp "+stamp.write(FORMAT_TIME_DATE)+" is "+stamp.write(FORMAT_UNIX)+" at actual time ");
    server_m->sendMessageToChannel(message.senderChannel(), "Current timestamp is UNIX:"+Timestamp().write(FORMAT_UNIX)+" STAMP:"+Timestamp().write(FORMAT_TIME_DATE));
}

void Bot::HandleSeriesCommands(const Message &message)
{
    QStringList word = message.content().split(" ");
    if (word[1] == LIST)
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
    }
    else if (word[1] == RELOAD)
    {
        SeriesParser(ROUTE_SERIES_FILE, ROUTE_ERROR_FILE, lSeries_m);
        server_m->sendMessageToChannel(message.senderChannel(), "Series reloaded succesfully");
    }
    else if (Series* series = GetSeries(word[1]))
    {
        if (word[2] == NEXT)
        {
            if (Episode* nextEp = series->GetNextEpisode())
            {
                server_m->sendMessageToChannel(message.senderChannel(), ".:"+series->GetMainTitle()+" - next episode:.");
                server_m->sendMessageToChannel(message.senderChannel(), nextEp->GetAirString());
            }
            else
                server_m->sendMessageToChannel(message.senderChannel(), "No more episodes to be aired for series "+series->GetMainTitle()+"!");
        }
        else if (word[2] == INFO)
        {
            if (!series->GetInfo().isEmpty())
            {
                server_m->sendMessageToChannel(message.senderChannel(), ".:"+series->GetMainTitle()+" - info:.");
                server_m->sendMessageToChannel(message.senderChannel(), series->GetInfo());
            }
            else
                server_m->sendMessageToChannel(message.senderChannel(), "No info for series "+series->GetMainTitle()+" found!");
        }

        else if (word[2] == TITLES)
        {
            QString known = series->GetTitles().join(", ");
            if (!known.isEmpty())
                server_m->sendMessageToChannel(message.senderChannel(), "Known titles for series with codename '"+series->GetName()+"': "+known);
            else
                server_m->sendMessageToChannel(message.senderChannel(), "Series is known only by its codename '"+series->GetName()+"'");
        }
        else
        {
            EpisodeOrder epOrder(word[2]);
            if (!epOrder.isSet())
                return;

            if (Episode* episode = series->GetEpisodeByOrder(epOrder))
            {
                server_m->sendMessageToChannel(message.senderChannel(), episode->GetAirString());
                if (word.size() == 4 && word[3] == INFO)
                {
                    if (!episode->GetInfo().isEmpty())
                    {
                        server_m->sendMessageToUser(message.senderNick(), "---!!! SPOILER ALERT !!!---");
                        server_m->sendMessageToUser(message.senderNick(), episode->GetInfo());
                        server_m->sendMessageToUser(message.senderNick(), "---!!! SPOILER ALERT !!!---");
                    }
                    else
                        server_m->sendMessageToUser(message.senderNick(), "No info for episode "+epOrder.GetNormalLook()+" found!");
                }
            }
            else
                server_m->sendMessageToChannel(message.senderChannel(), "Episode "+epOrder.GetNormalLook()+" not found!");
        }
    }
}

void Bot::HandleProfileCommands(const Message &message)
{
    QStringList word = message.content().split(" ");
    if (word[1] == "add")
    {
        if (word.size() > 3)
        {
            QString& profileName = word[2];
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
                profile->SetSeries(message.content().mid(message.content().indexOf(word[3])), lSeries_m);
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
    else if (Profile* profile = profileMgr->GetProfile(word[1]))
    {
        if (word.size() < 3)
            return;

        if (word[2] == "next")
        {
            if (word.size() != 3)
                return;

            foreach(Series* series, profile->GetProfileSeries())
                if (Episode* ep = series->GetNextEpisode())
                    server_m->sendMessageToChannel(message.senderChannel(), series->GetMainTitle()+": "+ep->GetAirString());
        }
        else if (word[2] == "pass")
        {
            if (word.size() == 4)
            {
                if (!profile->GetPassHash().isEmpty())
                {
                    server_m->sendMessageToChannel(message.senderChannel(), "Password already set, if you wish to change it please type 'profile "+profile->GetName()+" pass old new'");
                    return;
                }

                profile->SetPassHash(isSha1Hash(word[3]) ? word[3] : getHashFor(word[3]));
                server_m->sendMessageToChannel(message.senderChannel(), "Password succesfully set!");
            }
            else if (word.size() == 5)
            {
                if (profile->GetPassHash().isEmpty())
                {
                    server_m->sendMessageToChannel(message.senderChannel(), "Cannot change password when none is set!");
                    return;
                }

                if (!profile->IsPassCorrect(word[3]))
                {
                    server_m->sendMessageToChannel(message.senderChannel(), "Wrong password!");
                    return;
                }

                profile->SetPassHash(isSha1Hash(word[4]) ? word[4] : getHashFor(word[4]));
                server_m->sendMessageToChannel(message.senderChannel(), "Password succesfully edited!");
            }
            else
                return;

            profileMgr->Save();
        }
        else if (word[2] == "edit")
        {
            if (word.size() < 5)
                return;

            if (!profile->IsPassCorrect(word[3]))
            {
                server_m->sendMessageToChannel(message.senderChannel(), "Wrong password!");
                return;
            }

            profile->SetSeries(message.content().mid(message.content().indexOf(word[4])), lSeries_m);

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

void Bot::HandleHashCommand(const Message &message)
{
    if (message.content().count(" ") != 1)
        return;

    server_m->sendMessageToChannel(message.senderChannel(), getHashFor(message.content().mid(message.content().indexOf(" "))));
}

bool Bot::CanAccessSeriesCommands(const Message &message)
{
    if (message.isPrivate())
        return true;

    QString command = message.content().left(message.content().indexOf(" ")).trimmed();
    if (command != COMPARE && command != SERIES && command != PROFILE && command != HASH)
        return true;

    if ((muteMap_m.value(message.senderChannel())+MUTE_TIME) < uint(time(0)))
    {
        muteMap_m[message.senderChannel()] = uint(time(0));
        return true;
    }
    return false;
}
