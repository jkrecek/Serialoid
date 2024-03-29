#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QStringList>
#include "episodeorder.h"

class ProfileMgr;

enum SeriesCommand
{
    COMMAND_NONE = 0,
    COMMAND_COMPARE,
    COMMAND_SERIES,
    COMMAND_PROFILE,
    COMMAND_HASH,
    COMMAND_LIST,
    COMMAND_RELOAD,
    COMMAND_NEXT,
    COMMAND_TODAY,
    COMMAND_TOMORROW,
    COMMAND_INFO,
    COMMAND_TITLES,
    COMMAND_ADD,
    COMMAND_PASS,
    COMMAND_EDIT,
    COMMAND_IS_SERIES,
    COMMAND_IS_PROFILE,
    COMMAND_EP_ORDER,
    COMMAND_RATING
};

class CommandParser
{
    public:
        CommandParser(QString _message);

        QString GetMessagePart(uint pos) const;
        SeriesCommand GetCommandOnPos(uint pos) const;

        bool IsCommand(uint pos) const { return GetCommandOnPos(pos) != COMMAND_NONE; }
        uint GetSize() const { return uint(parsedMessage_m.size()); }

    private:
        QString     message_m;
        QStringList parsedMessage_m;
};

#endif // COMMANDPARSER_H
