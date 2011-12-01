#include "commandparser.h"
#include "profilemgr.h"
#include "seriesmgr.h"

CommandParser::CommandParser(QString _message) : message_m(_message)
{
    parsedMessage_m = _message.split(" ");
}

SeriesCommand CommandParser::GetCommandOnPos(uint pos) const
{
    if (GetSize() < pos)
        return COMMAND_NONE;

    const QString& commandString = GetMessagePart(pos);
    if (commandString.isNull())                         return COMMAND_NONE;
    else if (commandString == "compare")                return COMMAND_COMPARE;
    else if (commandString == "series")                 return COMMAND_SERIES;
    else if (commandString == "profile")                return COMMAND_PROFILE;
    else if (commandString == "hash")                   return COMMAND_HASH;
    else if (commandString == "list")                   return COMMAND_LIST;
    else if (commandString == "reload")                 return COMMAND_RELOAD;
    else if (commandString == "next")                   return COMMAND_NEXT;
    else if (commandString == "today")                  return COMMAND_TODAY;
    else if (commandString == "tomorrow")               return COMMAND_TOMORROW;
    else if (commandString == "info")                   return COMMAND_INFO;
    else if (commandString == "titles")                 return COMMAND_TITLES;
    else if (commandString == "add")                    return COMMAND_ADD;
    else if (commandString == "pass")                   return COMMAND_PASS;
    else if (commandString == "edit")                   return COMMAND_EDIT;
    else if (sSeries.GetMap().contains(commandString))  return COMMAND_IS_SERIES;
    else if (sProfile.HasProfile(commandString))        return COMMAND_IS_PROFILE;
    else if (EpisodeOrder(commandString).isSet())       return COMMAND_EP_ORDER;
    else return COMMAND_NONE;
}

QString CommandParser::GetMessagePart(uint pos) const
{
    if (GetSize() < pos)
        return QString::null;

    return parsedMessage_m[pos-1];
}
