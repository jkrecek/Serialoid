QT += network
CONFIG += console
TEMPLATE = app
INCLUDEPATH += Constants Helper IRC Series Profile
HEADERS += Constants/ircconstants.h \
    Constants/Formulas.h \
    Helper/timestamp.h \
    Helper/seriesparser.h \
    Helper/messageparser.h \
    Helper/episodeorder.h \
    Helper/commandparser.h \
    IRC/user.h \
    IRC/spambot.h \
    IRC/message.h \
    IRC/ircserver.h \
    IRC/channel.h \
    Series/series.h \
    Series/season.h \
    Series/episode.h \
    Profile/profilemgr.h \
    Profile/profile.h

SOURCES += main.cpp \
    Helper/commandparser.cpp \
    Helper/timestamp.cpp \
    Helper/seriesparser.cpp \
    Helper/messageparser.cpp \
    IRC/user.cpp \
    IRC/spambot.cpp \
    IRC/message.cpp \
    IRC/ircserver.cpp \
    IRC/channel.cpp \
    Profile/profilemgr.cpp
