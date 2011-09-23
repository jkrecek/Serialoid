#include <QList>
#include <QTextCodec>
#include "Formulas.h"
#include "channel.h"
#include "ircconstants.h"
#include "ircserver.h"
#include "messageparser.h"
#include "spambot.h"
#include "timestamp.h"

IRCServer::IRCServer(const QString& serverAddress, quint16 serverPort, QObject* parent) :
    QObject(parent),
    serverAddress_m(serverAddress),
    serverPort_m(serverPort),
    socket_m(0),
    loggedIn_m(false),
    codecName_m("UTF-8"),
    codec_m(QTextCodec::codecForName(codecName_m))
{

}

IRCServer::~IRCServer()
{
    disconnect();
    socket_m->deleteLater();
    qDeleteAll(joinedChannels_m);
}

void IRCServer::connectAs(const QString& nickName, const QString& userName, const QString& realName, const QString& userPassword)
{
    nickName_m = nickName;
    userName_m = userName;
    realName_m = realName;
    userPassword_m = userPassword;

    connectToServer();
}

void IRCServer::connectToServer()
{
    socket_m = new QTcpSocket(this);

    connect(socket_m, SIGNAL(connected()), this, SLOT(login()));
    connect(socket_m, SIGNAL(disconnected()), this, SLOT(disconnect()));
    connect(socket_m, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorOccured(QAbstractSocket::SocketError)));
    connect(socket_m, SIGNAL(readyRead()), this, SLOT(readData()));

    socket_m->connectToHost(serverAddress_m, serverPort_m);
}

void IRCServer::login()
{
    QByteArray data;
    data.append(IRC::PassIdentifyPrefix + " " + userPassword_m + "\n");
    data.append(IRC::NickIdentifyPrefix + " " + nickName_m + "\n");
    data.append(IRC::UserIdentifyPrefix + " " + userName_m + " cotoje tady :" + realName_m);
    send(data);
}

void IRCServer::joinChannel(const QString& channelName)
{
    addJoinChannelRequestToQueue(channelName);
}

void IRCServer::addJoinChannelRequestToQueue(const QString& channelName)
{
    QByteArray command = QByteArray("JOIN " + channelName.toUtf8());
    sendCommandAsap(command);
}

void IRCServer::changeNickName(const QString& nickName)
{
    addChangeNickNameRequestToQueue(nickName);
}

void IRCServer::addChangeNickNameRequestToQueue(const QString& nickName)
{
    QByteArray command = QByteArray("NICK " + nickName.toUtf8());
    sendCommandAsap(command);
}

void IRCServer::sendActionToChannel(const QString& channelName, const QString& actionContent)
{
    addChannelActionToQueue(channelName, actionContent);
}

void IRCServer::addChannelActionToQueue(const QString& channelName, const QString& actionContent)
{
    QByteArray command = "PRIVMSG " + channelName.toUtf8() + " :" + char(1) + "ACTION " + actionContent.toUtf8() + char(1);
    sendCommandAsap(command);
}

void IRCServer::sendActionToUser(const QString& userName, const QString& actionContent)
{
    addChannelMessageToQueue(userName, actionContent);
}

void IRCServer::addPrivateActionToQueue(const QString& userName, const QString& actionContent)
{
    QByteArray command = "PRIVMSG " + userName.toUtf8() + " :" + char(1) + "ACTION " + actionContent.toUtf8() + char(1);
    sendCommandAsap(command);
}

void IRCServer::sendMessageToChannel(const QString& channelName, const QString& messageContent)
{
    addChannelMessageToQueue(channelName, messageContent);
}

void IRCServer::addChannelMessageToQueue(const QString& channelName, const QString& messageContent)
{
    QByteArray begin = "PRIVMSG " + channelName.toUtf8() + " :";
    int allowedSize = MESSAGE_MAX - begin.size();

    QByteArray next = messageContent.toUtf8();
    while(!next.isNull())
    {
        if (next.size() <= allowedSize)
        {
            sendCommandAsap(begin+next.trimmed());
            break;
        }

        int idx = next.lastIndexOf(" ", allowedSize);
        sendCommandAsap(begin+next.left(idx).trimmed());
        next = next.mid(idx);
    }
}

void IRCServer::sendMessageToUser(const QString& userName, const QString& messageContent)
{
    addPrivateMessageToQueue(userName, messageContent);
}

void IRCServer::opSelf(const QString& channel)
{
    QString message = "OP "+channel;
    addPrivateMessageToQueue("chanserv", message);
}

void IRCServer::identify(const QString &pass)
{
    QString message = "IDENTIFY "+pass;
    addPrivateMessageToQueue("nickserv", message);
}

void IRCServer::addPrivateMessageToQueue(const QString& userName, const QString& messageContent)
{
    QByteArray begin = "PRIVMSG " + userName.toUtf8() + " :";
    int allowedSize = MESSAGE_MAX - begin.size();

    QByteArray next = messageContent.toUtf8();
    while(!next.isNull())
    {
        sendCommandAsap(begin+next.left(next.lastIndexOf(" ", allowedSize)));
        next = next.mid(next.lastIndexOf(" ", allowedSize));
    }
}

void IRCServer::opUser(const QString& nick, const QString& channel, bool apply)
{
    QByteArray command = "MODE " + channel.toUtf8() + " "+ (apply ? "+" : "-") + "o " + nick.toUtf8();
    sendCommandAsap(command);
}


void IRCServer::kick(const QString &nick, const QString &channel, const QString& reason)
{
    QByteArray command = "KICK " + channel.toUtf8() + " " + nick.toUtf8();

    if (!reason.isEmpty())
        command += QByteArray(" :") + reason.toUtf8();

    sendCommandAsap(command);
}

void IRCServer::sendCommandAsap(const QByteArray& command)
{
    if (isLoggedIn())
        send(command.trimmed());
    else
    {
        commandsInQueue_m.push_back(command.trimmed());
        enqueuedCommand();
    }
}

void IRCServer::sendCommandsInQueue()
{
    if (isLoggedIn())
    {
        while(!commandsInQueue_m.isEmpty())
        {
            QByteArray command = commandsInQueue_m.takeFirst();
            send(command);
        }
    }
}

void IRCServer::enqueuedCommand()
{
    if (loggedIn_m)
        sendCommandsInQueue();
}

void IRCServer::sendMessage(const QString& rawText)
{
    QByteArray rawData = rawText.toUtf8();
    send(rawData);
}

void IRCServer::send(const QByteArray& rawData)
{
    printf("SND '%s': '%s'\n", Timestamp().write(FORMAT_TIME).toStdString().c_str(), rawData.data());
    socket_m->write(rawData + "\n");
    socket_m->flush();
;
}

void IRCServer::errorOccured(QAbstractSocket::SocketError)
{
    // Have to read about the possible errors first
}

void IRCServer::readData()
{
    while (socket_m->canReadLine())
    {
        QByteArray messageInRawText = socket_m->readLine();
        printf("RCV '%s': '%s'\n", Timestamp().write(FORMAT_TIME).toStdString().c_str(), messageInRawText.trimmed().data());
        Message message = MessageParser::receivedRawTextToMessage(messageInRawText);

        emit messageReceived(message);

        if (message.isServerMessage())
            handleServerMessage(message);
        else if (message.isChannelMessage())
            handleChannelMessage(message);
    }
}

void IRCServer::handleServerMessage(const Message& message)
{
    // Receivied Ping
    if (message.isPing())
        sendMessage("PONG :" + message.content());

    // Successfully logged in
    else if (message.command() == IRC::Reply::WELCOME)
    {
        loggedIn_m = true;
        sendCommandsInQueue();
        emit loginAccepted();
    }
}

void IRCServer::handleChannelMessage(const Message& message)
{
    // Successfully joined to channel
    if (message.command() == IRC::Command::Join)
    {
        // Channel in JOIN command starts with :#
        QString channelName = message.senderChannel().remove(0, 2);
        joinedChannels_m[channelName] = new Channel(channelName, this);
    }
}

QByteArray IRCServer::encode(const QString& text)
{
    QTextEncoder* encoder = codec_m->makeEncoder();
    return encoder->fromUnicode(text);
}

void IRCServer::setEncoding(const QByteArray& codecName)
{
    if (codecName != codecName_m)
    {
        codecName_m = codecName;
        codec_m = QTextCodec::codecForName(codecName_m);
    }
}

Channel* IRCServer::channel(const QString& channelName)
{
    return joinedChannels_m.value(channelName, 0);
}

bool IRCServer::isLoggedIn() const
{
    return loggedIn_m;
}

void IRCServer::disconnect(const QByteArray& quitMessage)
{
    if (socket_m->isOpen())
    {
        send("QUIT :" + quitMessage);
        socket_m->close();
    }
}
