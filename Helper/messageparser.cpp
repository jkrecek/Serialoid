#include <QStringList>
#include <QTextCodec>
#include "Formulas.h"
#include "ircconstants.h"
#include "messageparser.h"
#include "timestamp.h"

MessageParser::MessageParser()
{
}

Message MessageParser::receivedRawTextToMessage(const QByteArray& rawText, const QByteArray& defaultCodec)
{
    Message message;

    QByteArray trimmedRawText = rawText.trimmed();
    QString unicodeRawText = decodeRawText(trimmedRawText, defaultCodec);

    QStringList messageParts = unicodeRawText.split(' ', QString::SkipEmptyParts);

    QString senderNick = QString();
    QString senderChannel = QString();
    QString senderUser = QString();
    QString senderHost = QString();
    QString command = QString();
    QString content = QString();
    bool isServerMessage = false;
    bool isChannelMessage = false;

    if(!messageParts.isEmpty())
    {
        // Message has prefix
        if (messageParts[0].startsWith(IRC::MessagePrefix))
        {
            if (!messageParts[0].contains(IRC::UserPrefix))
                isServerMessage = true;
            else
            {
                senderNick = messageParts[0].mid(1, messageParts[0].indexOf(IRC::UserPrefix) - 1);
                senderUser = messageParts[0].mid(messageParts[0].indexOf("~") + 1, messageParts[0].indexOf("@")-messageParts[0].indexOf("~")-1);
                senderHost = messageParts[0].mid(messageParts[0].indexOf("@") + 1, messageParts[0].size()-messageParts[0].indexOf("@")-1);
            }

            // Remove prefix
            messageParts.removeFirst();
        }

        command = messageParts[0].toUpper();

        if(message.isServerMessage())
        {
            if ( messageParts.count() >= 3 )
            {
                if (command == IRC::Reply::TOPIC)
                    senderChannel = messageParts[2];
                else if (command == IRC::Reply::MOTD)
                    senderChannel = ":server";
            }
        }
        else if (messageParts.count() >= 2)
        {
            // Query
            if (command == IRC::Command::Ping ||
                command == IRC::Command::Error )
            {
                isServerMessage = true;
            }
            else if ((command == IRC::Command::PrivMsg || command == IRC::Command::Notice) && !messageParts[1].startsWith("#"))
                senderChannel = senderNick;
            // Channel message
            else if (command == IRC::Command::PrivMsg ||
                     command == IRC::Command::Notice  ||
                     command == IRC::Command::Join    ||
                     command == IRC::Command::Kick    ||
                     command == IRC::Command::Part    ||
                     command == IRC::Command::Topic    )
            {
                isChannelMessage = true;
                senderChannel = messageParts[1];
            }
        }
    }

    content = unicodeRawText.right(unicodeRawText.size() - unicodeRawText.indexOf(" :", 1) - 2);

    message.setIsServerMessage(isServerMessage);
    message.setIsChannelMessage(isChannelMessage);

    message.setSenderNick(senderNick.trimmed());
    //message.setSenderUser(senderUser.trimmed());
    //message.setSenderHost(senderHost.trimmed());
    message.setChannel(senderChannel.trimmed());
    message.setCommand(command.trimmed());
    message.setContent(content.trimmed());
    message.setSendTime();

    return message;
}

QString MessageParser::decodeRawText(const QByteArray& rawText, const QByteArray& defaultCodec)
{
    QTextCodec* codec = QTextCodec::codecForName(defaultCodec);
    QTextDecoder* decoder = codec->makeDecoder();

    return decoder->toUnicode(rawText);
}
