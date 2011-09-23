QT += network
CONFIG += console
TEMPLATE = app
HEADERS += Constants/ircconstants.h \
    Constants/Formulas.h \
    Helper/timestamp.h \
    Helper/setting.h \
    Helper/messageparser.h \
    Helper/episodeorder.h \
    IRC/user.h \
    IRC/spambot.h \
    IRC/message.h \
    IRC/ircserver.h \
    IRC/channel.h \
    Series/series.h \
    Series/season.h \
    Series/episode.h

SOURCES += Helper/timestamp.cpp \
    Helper/setting.cpp \
    Helper/messageparser.cpp \
    IRC/user.cpp \
    IRC/spambot.cpp \
    IRC/message.cpp \
    IRC/ircserver.cpp \
    IRC/channel.cpp

