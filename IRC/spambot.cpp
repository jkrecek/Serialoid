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
#include "seriesmgr.h"
#include "spambot.h"
#include "user.h"
#include "bottime.h"

#define ROUTE_SERIES_FILE "series.txt"
#define ROUTE_PROFILE_FILE "profile.txt"
#define ROUTE_ERROR_FILE "error.txt"

Bot::Bot(QObject* parent) : QObject(parent)
{
    // initialize time updating
    sBotTime;
    connect(&sBotTime, SIGNAL(Update(int)), this, SLOT(UpdateBot(int)));

    // connecting to servers
    server_m = new IRCServer("irc.rizon.net", 6667);
    server_m->connectAs("Serialoid", "BOT", "BOT", "Kurva_tahnite_mi_z_nicku");

    server_m->joinChannel("#SoulWell");
    connect(server_m, SIGNAL(messageReceived(Message)), this, SLOT(handleReceivedMessage(Message)));

    sSeries.Load(ROUTE_SERIES_FILE, ROUTE_ERROR_FILE);
    sProfile.Load(ROUTE_PROFILE_FILE, ROUTE_ERROR_FILE);

    // parser must be set after loading loacal files
    parser_m = new ParserMgr();
    connect(parser_m, SIGNAL(allParsed()), this, SLOT(parsingComplete()));

    qsrand(sqrt(time(0))*2);
}

void Bot::handleReceivedMessage(const Message& message)
{
    // rejoin on kick (TODO:: do not rejoin on every kick)
    if (message.command() == IRC::Command::Kick)
        server_m->joinChannel(message.senderChannel());

    command_m = new CommandParser(message.content());
    if (command_m->IsCommand(1))
    {
        if (uint endtime = GetCooldownEndTime(message))
            server_m->sendNoticeToUser(message.senderNick(), "I'm on cooldown for next "+Timestamp(endtime).getTo());
        else
        {
            bool noErrorOccured;
            switch(command_m->GetCommandOnPos(1))
            {
                case COMMAND_COMPARE:
                    noErrorOccured = HandleTimeComparison(message);
                    break;
                case COMMAND_SERIES:
                    noErrorOccured = HandleSeriesCommands(message);
                    break;
                case COMMAND_PROFILE:
                    noErrorOccured = HandleProfileCommands(message);
                    break;
                case COMMAND_HASH:
                    noErrorOccured = HandleHashCommand(message);
                    break;
                default:
                    break;
            }

            if (!noErrorOccured)
            {
                server_m->sendMessageToChannel(message.senderChannel(), "Sorry, Incorrect syntax");
                muteMap_m[message.senderChannel()] = 0;
            }
        }
    }
    delete command_m;
    command_m = NULL;

    if (message.senderNick() == server_m->ownNick() && message.content() == "quit")
        exit(1);

    if (!message.isServerMessage() && message.command() == IRC::Command::PrivMsg)
    {
        if (message.content().toLower().contains(server_m->ownNick().toLower()))
            server_m->sendMessageToChannel(message.senderChannel(), "Coe?!?!?!");
    }
}

bool Bot::HandleTimeComparison(const Message &message)
{
    Timestamp stamp(command_m->GetMessagePart(2), command_m->GetMessagePart(3));
    if (!stamp.isCorrect())
        return false;

    server_m->sendMessageToChannel(message.senderChannel(), "Unix time for timestamp "+stamp.write(FORMAT_TIME_DATE)+" is "+stamp.write(FORMAT_UNIX)+" at actual time ");
    server_m->sendMessageToChannel(message.senderChannel(), "Current timestamp is UNIX:"+Timestamp().write(FORMAT_UNIX)+" STAMP:"+Timestamp().write(FORMAT_TIME_DATE));
    return true;
}

bool Bot::HandleSeriesCommands(const Message &message)
{
    if (parser_m)       // if has valid pointer to parser then bot is currently parsing, disallow accesing to series
    {
        server_m->sendMessageToChannel(message.senderChannel(), "Sorry, I'm currently parsing, please ask me when I'm finished");
        return true;
    }

    switch(command_m->GetCommandOnPos(2))
    {
        case COMMAND_LIST:
        {
            if (sSeries.GetMap().empty())
            {
                server_m->sendMessageToChannel(message.senderChannel(), "No series found, please check your series.txt file.");
                return true;
            }

            QString known;
            foreach(Series* series, sSeries.GetMap())
            {
                if (!known.isEmpty())
                    known.append(", ");

                known.append(series->GetMainTitle()+" ("+series->GetName()+")");
            }
            server_m->sendMessageToChannel(message.senderChannel(), ".:Printing all known series ( format Name(codename) ):.");
            server_m->sendMessageToChannel(message.senderChannel(), known);
            return true;
        }
        case COMMAND_RELOAD:
        {
            sSeries.Load();
            server_m->sendMessageToChannel(message.senderChannel(), "Series reloaded succesfully, start parsing now");
            parser_m = new ParserMgr();
            connect(parser_m, SIGNAL(allParsed()), this, SLOT(parsingComplete()));
            return true;
        }
        case COMMAND_TODAY:
        {
            Timestamp morning("00:00:00", Timestamp().getDate());
            Timestamp evening(morning.getUnix()+DAY_S);

            bool found = false;
            foreach(Series* series, sSeries.GetMap())
            {
                foreach(Episode* ep, series->GetAllEpisodes())
                {
                    if (isInRange(morning.getUnix(), ep->GetAir().getUnix(), evening.getUnix()))
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), series->GetMainTitle()+": "+ep->GetAirString());
                        found = true;
                    }
                }
            }
            if (!found)
                server_m->sendMessageToChannel(message.senderChannel(), "No series is about to be aired today!");
            return true;
        }
        case COMMAND_TOMORROW:
        {
            Timestamp morning("23:59:59", Timestamp().getDate());
            Timestamp evening(morning.getUnix()+DAY_S);

            bool found = false;
            foreach(Series* series, sSeries.GetMap())
            {
                foreach(Episode* ep, series->GetAllEpisodes())
                {
                    if (isInRange(morning.getUnix(), ep->GetAir().getUnix(), evening.getUnix()))
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), series->GetMainTitle()+": "+ep->GetAirString());
                        found = true;
                    }
                }
            }
            if (!found)
                server_m->sendMessageToChannel(message.senderChannel(), "No series is about to be aired tomorrow!");

            return true;
        }
        case COMMAND_IS_SERIES:
        {
            Series* series = sSeries.GetSeries(command_m->GetMessagePart(2));
            switch(command_m->GetCommandOnPos(3))
            {
                case COMMAND_NEXT:
                {
                    if (Episode* nextEp = series->GetNextEpisode())
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), ".:"+series->GetMainTitle()+" - next episode:.");
                        server_m->sendMessageToChannel(message.senderChannel(), nextEp->GetAirString());
                    }
                    else
                        server_m->sendMessageToChannel(message.senderChannel(), "No more episodes to be aired for series "+series->GetMainTitle()+"!");

                    return true;
                }
                case COMMAND_INFO:
                {
                    if (!series->GetInfo().isEmpty())
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), ".:"+series->GetMainTitle()+" - info:.");
                        server_m->sendMessageToChannel(message.senderChannel(), series->GetInfo());
                    }
                    else
                        server_m->sendMessageToChannel(message.senderChannel(), "No info for series "+series->GetMainTitle()+" found!");

                    return true;
                }
                case COMMAND_TITLES:
                {
                    QString known = series->GetTitles().join(", ");
                    if (!known.isEmpty())
                        server_m->sendMessageToChannel(message.senderChannel(), "Known titles for series with codename '"+series->GetName()+"': "+known);
                    else
                        server_m->sendMessageToChannel(message.senderChannel(), "Series is known only by its codename '"+series->GetName()+"'");

                    return true;
                }
                case COMMAND_EP_ORDER:
                {
                    EpisodeOrder epOrder(command_m->GetMessagePart(3));
                    if (!epOrder.isSet())
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), "SOMETHING WENT TERRIBLY WRONG!");
                        return true;
                    }

                    if (Episode* episode = series->GetEpisodeByOrder(epOrder))
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), episode->GetAirString());
                        switch(command_m->GetCommandOnPos(4))
                        {
                            case COMMAND_INFO:
                            {
                                if (!episode->GetInfo().isEmpty())
                                {
                                    server_m->sendMessageToUser(message.senderNick(), "---!!! SPOILER ALERT !!!---");
                                    server_m->sendMessageToUser(message.senderNick(), episode->GetInfo());
                                    server_m->sendMessageToUser(message.senderNick(), "---!!! SPOILER ALERT !!!---");
                                }
                                else
                                    server_m->sendMessageToUser(message.senderNick(), "No info for episode "+epOrder.GetNormalLook()+" found!");

                                return true;
                            }
                            case COMMAND_RATING:
                            {
                                const float& rating = episode->GetRating();
                                if (rating)
                                    server_m->sendMessageToChannel(message.senderChannel(), "Episode rating according to TV.com is "+QString::number(rating));
                                else
                                    server_m->sendMessageToChannel(message.senderChannel(), "Episode rating not found");

                                return true;
                            }
                            default:
                                return true;
                        }
                    }
                    else
                        server_m->sendMessageToChannel(message.senderChannel(), "Episode "+epOrder.GetNormalLook()+" not found!");

                    return true;
                }
                default:
                    return false;
            }
            return false;
        }
        default:
            return false;
    }
}

bool Bot::HandleProfileCommands(const Message &message)
{
    if (parser_m)       // if has valid pointer to parser then bot is currently parsing, disallow accesing to series
    {
        server_m->sendMessageToChannel(message.senderChannel(), "Sorry, I'm currently parsing, please ask me when I'm finished");
        return true;
    }

    switch(command_m->GetCommandOnPos(2))
    {
        case COMMAND_ADD:
        {
            const QString profileName = command_m->GetMessagePart(3);
            if (profileName.isEmpty())
            {
                server_m->sendMessageToChannel(message.senderChannel(), "Profile name cannot be empty!");
                return true;
            }
            if (sProfile.isNameForbidden(profileName))
            {
                server_m->sendMessageToChannel(message.senderChannel(), "You cannot use profile name "+profileName+". Such name is forbidden!");
                return true;
            }
            Profile* profile = sProfile.GetProfile(profileName);
            if (!profile)
            {
                profile = new Profile(profileName);
                profile->SetSeries(message.content().mid(message.content().indexOf(command_m->GetMessagePart(4))));
                sProfile.AddProfile(profile);

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

            return true;
        }
        case COMMAND_IS_PROFILE:
        {
            Profile* profile = sProfile.GetProfile(command_m->GetMessagePart(2));
            switch(command_m->GetCommandOnPos(3))
            {
                case COMMAND_NEXT:
                {
                    foreach(Series* series, profile->GetProfileSeries())
                        if (Episode* ep = series->GetNextEpisode())
                            server_m->sendMessageToChannel(message.senderChannel(), series->GetMainTitle()+": "+ep->GetAirString());

                    return true;
                }
                case COMMAND_TODAY:
                {
                    Timestamp morning("00:00:00", Timestamp().getDate());
                    Timestamp evening(morning.getUnix()+DAY_S);

                    bool found = false;
                    foreach(Series* series, profile->GetProfileSeries())
                    {
                        foreach(Episode* ep, series->GetAllEpisodes())
                        {
                            if (isInRange(morning.getUnix(), ep->GetAir().getUnix(), evening.getUnix()))
                            {
                                server_m->sendMessageToChannel(message.senderChannel(), series->GetMainTitle()+": "+ep->GetAirString());
                                found = true;
                            }
                        }
                    }
                    if (!found)
                        server_m->sendMessageToChannel(message.senderChannel(), "None of your favorite series is about to be aired today!");

                    return true;
                }
                case COMMAND_TOMORROW:
                {
                    Timestamp morning("23:59:59", Timestamp().getDate());
                    Timestamp evening(morning.getUnix()+DAY_S);

                    bool found = false;
                    foreach(Series* series, profile->GetProfileSeries())
                    {
                        foreach(Episode* ep, series->GetAllEpisodes())
                        {
                            if (isInRange(morning.getUnix(), ep->GetAir().getUnix(), evening.getUnix()))
                            {
                                server_m->sendMessageToChannel(message.senderChannel(), series->GetMainTitle()+": "+ep->GetAirString());
                                found = true;
                            }
                        }
                    }
                    if (!found)
                        server_m->sendMessageToChannel(message.senderChannel(), "None of your favorite series is about to be aired tommorow!");

                    return true;
                }
                case COMMAND_PASS:
                {
                    switch(command_m->GetSize())
                    {
                        case 4:
                        {
                            if (!profile->GetPassHash().isEmpty())
                            {
                                server_m->sendMessageToChannel(message.senderChannel(), "Password already set, if you wish to change it please type 'profile "+profile->GetName()+" pass old new'");
                                return true;
                            }
                            QString pass = command_m->GetMessagePart(4);
                            if (!pass.isEmpty())
                            {
                                profile->SetPassHash(isSha1Hash(pass) ? pass : getHashFor(pass));
                                server_m->sendMessageToChannel(message.senderChannel(), "Password succesfully set!");
                            }
                            break;
                        }
                        case 5:
                        {
                            if (profile->GetPassHash().isEmpty())
                            {
                                server_m->sendMessageToChannel(message.senderChannel(), "Cannot change password when none is set!");
                                return true;
                            }

                            if (!profile->IsPassCorrect(command_m->GetMessagePart(4)))
                            {
                                server_m->sendMessageToChannel(message.senderChannel(), "Wrong password!");
                                return true;
                            }

                            QString pass = command_m->GetMessagePart(5);
                            if (!pass.isEmpty())
                            {
                                profile->SetPassHash(isSha1Hash(pass) ? pass : getHashFor(pass));
                                server_m->sendMessageToChannel(message.senderChannel(), "Password succesfully edited!");
                            }
                            break;
                        }
                        default:
                        {
                            server_m->sendMessageToChannel(message.senderChannel(), "Command 'profile "+profile->GetName()+" pass' have to have 4 or 5 arguments!");
                            return true;
                        }
                    }
                    sProfile.Save();
                    return true;
                }
                case COMMAND_EDIT:
                {
                    if (!profile->IsPassCorrect(command_m->GetMessagePart(4)))
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), "Wrong password!");
                        return true;
                    }

                    QString seriesStart = command_m->GetMessagePart(5);
                    if (seriesStart.isEmpty())
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), "If you wish to change series in your profile, you have to enter new ones!");
                        return true;
                    }

                    profile->SetSeries(message.content().mid(message.content().indexOf(seriesStart)));

                    QString s;
                    foreach(Series* series, profile->GetProfileSeries())
                    {
                        if (!s.isEmpty())
                            s.append(", ");

                        s.append(series->GetMainTitle()+"("+series->GetName()+")");
                    }
                    server_m->sendMessageToChannel(message.senderChannel(), "Succesfully edited profile '"+profile->GetName()+"' containing series: "+s);
                    return true;
                }
                default:
                    return false;
            }
            return false;
        }
        default:
            return false;
    }
}

bool Bot::HandleHashCommand(const Message &message)
{
    if (command_m->GetSize() != 2)
        return false;

    server_m->sendMessageToChannel(message.senderChannel(), getHashFor(message.content().mid(message.content().indexOf(" "))));
    return true;
}

uint Bot::GetCooldownEndTime(const Message &message)
{
    if (message.isPrivate())
        return 0;

    if (muteMap_m.value(message.senderChannel()) > uint(time(0)))
        return muteMap_m.value(message.senderChannel());

    muteMap_m[message.senderChannel()] = uint(time(0))+MUTE_TIME;
    return 0;
}

void Bot::UpdateBot(const int diff)
{
    int toSecs = float(diff)/1000;
    foreach(Series* series, sSeries.GetMap())
        foreach(Episode* ep, series->GetAllEpisodes())
            if (ep->GetAir().timeFrom() < toSecs)
                server_m->sendMessageToChannel("#soulwell", series->GetMainTitle()+": "+ep->GetJustAiredString());
}

void Bot::parsingComplete()
{
    foreach(Channel* channel, server_m->GetAllJoinedChannels())
        server_m->sendMessageToChannel("#"+channel->getName(), "All series loaded!");

    // parser is not needed anymore, we can delete it now
    parser_m->deleteLater();
    parser_m = NULL;
}
