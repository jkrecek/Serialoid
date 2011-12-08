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

#define ROUTE_SERIES_FILE "C://series.txt"
#define ROUTE_PROFILE_FILE "C://profile.txt"
#define ROUTE_ERROR_FILE "C://error.txt"

Bot::Bot(QObject* parent) : QObject(parent)
{
    sTime.start();

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

    command = new CommandParser(message.content());
    if (command->IsCommand(1))
    {
        if (CanAccessSeriesCommands(message))
        {
            switch(command->GetCommandOnPos(1))
            {
                case COMMAND_COMPARE:
                    HandleTimeComparison(message);
                    break;
                case COMMAND_SERIES:
                    HandleSeriesCommands(message);
                    break;
                case COMMAND_PROFILE:
                    HandleProfileCommands(message);
                    break;
                case COMMAND_HASH:
                    HandleHashCommand(message);
                    break;
                default:
                    break;
            }
        }
    }
    delete command;
    command = NULL;

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
    Timestamp stamp(command->GetMessagePart(2), command->GetMessagePart(3));
    server_m->sendMessageToChannel(message.senderChannel(), "Unix time for timestamp "+stamp.write(FORMAT_TIME_DATE)+" is "+stamp.write(FORMAT_UNIX)+" at actual time ");
    server_m->sendMessageToChannel(message.senderChannel(), "Current timestamp is UNIX:"+Timestamp().write(FORMAT_UNIX)+" STAMP:"+Timestamp().write(FORMAT_TIME_DATE));
}

void Bot::HandleSeriesCommands(const Message &message)
{
    if (parser_m)       // if has valid pointer to parser then bot is currently parsing, disallow accesing to series
    {
        server_m->sendMessageToChannel(message.senderChannel(), "Sorry, I'm currently parsing, please ask me when I'm finished");
        return;
    }

    switch(command->GetCommandOnPos(2))
    {
        case COMMAND_LIST:
        {
            QString known;
            foreach(Series* series, sSeries.GetMap())
            {
                if (!known.isEmpty())
                    known.append(", ");

                known.append(series->GetMainTitle()+" ("+series->GetName()+")");
            }
            server_m->sendMessageToChannel(message.senderChannel(), ".:Printing all known series ( format Name(codename) ):.");
            server_m->sendMessageToChannel(message.senderChannel(), known);
            break;
        }
        case COMMAND_RELOAD:
        {
            sSeries.Load();
            server_m->sendMessageToChannel(message.senderChannel(), "Series reloaded succesfully, start parsing now");
            parser_m = new ParserMgr();
            break;
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
            break;
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
            break;
        }
        case COMMAND_IS_SERIES:
        {
            Series* series = sSeries.GetSeries(command->GetMessagePart(2));
            switch(command->GetCommandOnPos(3))
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

                    break;
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

                    break;
                }
                case COMMAND_TITLES:
                {
                    QString known = series->GetTitles().join(", ");
                    if (!known.isEmpty())
                        server_m->sendMessageToChannel(message.senderChannel(), "Known titles for series with codename '"+series->GetName()+"': "+known);
                    else
                        server_m->sendMessageToChannel(message.senderChannel(), "Series is known only by its codename '"+series->GetName()+"'");

                    break;
                }
                case COMMAND_EP_ORDER:
                {
                    EpisodeOrder epOrder(command->GetMessagePart(3));
                    if (!epOrder.isSet())
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), "SOMETHING WENT TERRIBLY WRONG!");
                        break;
                    }

                    if (Episode* episode = series->GetEpisodeByOrder(epOrder))
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), episode->GetAirString());
                        if (command->GetCommandOnPos(4) == COMMAND_INFO)
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
                        if (command->GetCommandOnPos(4) == COMMAND_RATING)
                        {
                            const float& rating = episode->GetRating();
                            if (rating)
                                server_m->sendMessageToChannel(message.senderChannel(), "Episode rating according to TV.com is "+QString::number(rating));
                            else
                                server_m->sendMessageToChannel(message.senderChannel(), "Episode rating not found");
                        }
                    }
                    else
                        server_m->sendMessageToChannel(message.senderChannel(), "Episode "+epOrder.GetNormalLook()+" not found!");

                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

void Bot::HandleProfileCommands(const Message &message)
{
    if (parser_m)       // if has valid pointer to parser then bot is currently parsing, disallow accesing to series
    {
        server_m->sendMessageToChannel(message.senderChannel(), "Sorry, I'm currently parsing, please ask me when I'm finished");
        return;
    }

    switch(command->GetCommandOnPos(2))
    {
        case COMMAND_ADD:
        {
            const QString profileName = command->GetMessagePart(3);
            if (profileName.isEmpty())
            {
                server_m->sendMessageToChannel(message.senderChannel(), "Profile name cannot be empty!");
                return;
            }
            if (sProfile.isNameForbidden(profileName))
            {
                server_m->sendMessageToChannel(message.senderChannel(), "You cannot use profile name "+profileName+". Such name is forbidden!");
                return;
            }
            Profile* profile = sProfile.GetProfile(profileName);
            if (!profile)
            {
                profile = new Profile(profileName);
                profile->SetSeries(message.content().mid(message.content().indexOf(command->GetMessagePart(4))));
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

            break;
        }
        case COMMAND_IS_PROFILE:
        {
            Profile* profile = sProfile.GetProfile(command->GetMessagePart(2));
            switch(command->GetCommandOnPos(3))
            {
                case COMMAND_NEXT:
                {
                    foreach(Series* series, profile->GetProfileSeries())
                        if (Episode* ep = series->GetNextEpisode())
                            server_m->sendMessageToChannel(message.senderChannel(), series->GetMainTitle()+": "+ep->GetAirString());

                    break;
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
                    break;
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
                    break;
                }
                case COMMAND_PASS:
                {
                    switch(command->GetSize())
                    {
                        case 4:
                        {
                            if (!profile->GetPassHash().isEmpty())
                            {
                                server_m->sendMessageToChannel(message.senderChannel(), "Password already set, if you wish to change it please type 'profile "+profile->GetName()+" pass old new'");
                                break;
                            }
                            QString pass = command->GetMessagePart(4);
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
                                return;
                            }

                            if (!profile->IsPassCorrect(command->GetMessagePart(4)))
                            {
                                server_m->sendMessageToChannel(message.senderChannel(), "Wrong password!");
                                return;
                            }

                            QString pass = command->GetMessagePart(5);
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
                            break;
                        }
                    }
                    sProfile.Save();
                    break;
                }
                case COMMAND_EDIT:
                {
                    if (!profile->IsPassCorrect(command->GetMessagePart(4)))
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), "Wrong password!");
                        return;
                    }

                    QString seriesStart = command->GetMessagePart(5);
                    if (seriesStart.isEmpty())
                    {
                        server_m->sendMessageToChannel(message.senderChannel(), "If you wish to change series in your profile, you have to enter new ones!");
                        break;
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
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

void Bot::HandleHashCommand(const Message &message)
{
    if (command->GetSize() != 2)
        return;

    server_m->sendMessageToChannel(message.senderChannel(), getHashFor(message.content().mid(message.content().indexOf(" "))));
}

bool Bot::CanAccessSeriesCommands(const Message &message)
{
    if (message.isPrivate())
        return true;

    if ((muteMap_m.value(message.senderChannel())+MUTE_TIME) < uint(time(0)))
    {
        muteMap_m[message.senderChannel()] = uint(time(0));
        return true;
    }
    return false;
}

void Bot::parsingComplete()
{
    foreach(Channel* channel, server_m->GetAllJoinedChannels())
        server_m->sendMessageToChannel("#"+channel->getName(), "All series loaded!");

    // parser is not needed anymore, we can delete it now
    parser_m->deleteLater();
    parser_m = NULL;
}
