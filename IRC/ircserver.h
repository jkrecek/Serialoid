#ifndef IRCSERVER_H
#define IRCSERVER_H

#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include "message.h"
#include "spambot.h"

class Channel;
class QTextCodec;

enum Bans
{
    BAN_NONE            = 0x00,
    BAN_NICK            = 0x01,
    BAN_USER            = 0x02,
    BAN_HOST            = 0x04
};

#define MESSAGE_MAX 471

class IRCServer : public QObject
{
        Q_OBJECT

    public:
        IRCServer(const QString& serverAdress, quint16 serverPort = 6667, QObject* parent = 0);
        ~IRCServer();

        bool isLoggedIn() const;
        Channel* channel(const QString& channelName);
        QString ownNick() const { return nickName_m; }

    public slots:
        void connectAs(const QString& nickName, const QString& userName, const QString& realName, const QString& userPassword);
        void changeNickName(const QString& nickName);
        void sendMessageToChannel(const QString& channelName, const QString& messageContent);
        void sendMessageToUser(const QString& userName, const QString& messageContent);
        void sendActionToChannel(const QString& channelName, const QString& messageContent);
        void sendActionToUser(const QString& userName, const QString& messageContent);
        void send(const QByteArray& rawData);
        void joinChannel(const QString& channelName);
        void setEncoding(const QByteArray& codecName);
        void connectToServer();
        void disconnect(const QByteArray& quitMessage = QByteArray("Disconnected for unknown reason"));
        void opUser(const QString& nick, const QString& channel, bool apply);
        void opSelf(const QString& channel);
        void identify(const QString& pass);
        void kick(const QString& nick, const QString& channel, const QString& reason = QString());

    signals:
        void messageReceived(const Message& message);
        void loginAccepted();

    private slots:
        void errorOccured(QAbstractSocket::SocketError);
        void readData();
        void login();

    private:
        void sendMessage(const QString& rawText);

        void addChannelMessageToQueue(const QString& channelName, const QString& messageContent);
        void addPrivateMessageToQueue(const QString& userName, const QString& messageContent);
        void addChannelActionToQueue(const QString& channelName, const QString& messageContent);
        void addPrivateActionToQueue(const QString& userName, const QString& messageContent);
        void addJoinChannelRequestToQueue(const QString& channelName);
        void addChangeNickNameRequestToQueue(const QString& nickName);

        void handleServerMessage(const Message& message);
        void handleChannelMessage(const Message& message);
        void sendCommandAsap(const QByteArray& command);
        void sendCommandsInQueue();
        void enqueuedCommand();

        QByteArray encode(const QString& text);

        QString serverAddress_m;
        quint16 serverPort_m;
        QTcpSocket* socket_m;

        QString nickName_m;
        QString userName_m;
        QString realName_m;
        QString userPassword_m;
        bool loggedIn_m;

        QByteArray codecName_m;
        QTextCodec* codec_m;

        QList<QByteArray> commandsInQueue_m;

        QMap<QString, Channel*> joinedChannels_m;
};

#endif
