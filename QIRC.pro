QT += network
CONFIG += console
TEMPLATE = app
HEADERS += ircserver.h \
    message.h \
    channel.h \
    messageparser.h \
    user.h \
    ircconstants.h \
    spambot.h \
    Formulas.h \
    series.h \
    episode.h \
    season.h \
    episodeorder.h \
    setting.h \
    timestamp.h

SOURCES += main.cpp \
    message.cpp \
    channel.cpp \
    messageparser.cpp \
    user.cpp \
    ircserver.cpp \
    spambot.cpp \
    setting.cpp \
    timestamp.cpp
