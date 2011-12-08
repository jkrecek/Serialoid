QT += network
CONFIG += console
TEMPLATE = app
INCLUDEPATH += Constants Helper IRC Series Profile WebParser
HEADERS += Constants/Formulas.h \
    Constants/ircconstants.h \
    Helper/commandparser.h \
    Helper/episodeorder.h \
    Helper/messageparser.h \
    Helper/singleton.h \
    Helper/timestamp.h \
    IRC/ircserver.h \
    IRC/channel.h \
    IRC/message.h \
    IRC/spambot.h \
    IRC/urldownloader.h \
    IRC/updater.h \
    IRC/user.h \
    Series/episode.h \
    Series/season.h \
    Series/series.h \
    Series/seriesmgr.h \
    Profile/profilemgr.h \
    Profile/profile.h \
    WebParser/parsermgr.h


SOURCES += main.cpp \
    Helper/commandparser.cpp \
    Helper/messageparser.cpp \
    Helper/timestamp.cpp \
    IRC/ircserver.cpp \
    IRC/channel.cpp \
    IRC/message.cpp \
    IRC/spambot.cpp \
    IRC/urldownloader.cpp \
    IRC/user.cpp \
    IRC/updater.cpp \
    Profile/profilemgr.cpp \
    Series/series.cpp \
    Series/seriesmgr.cpp \
    WebParser/parsermgr.cpp
