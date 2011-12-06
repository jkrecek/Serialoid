#ifndef TVPARSER_H
#define TVPARSER_H

#include "basicparser.h"

class TVParser : public BasicParser
{
    public:
        TVParser(Series *_s, const QByteArray _c);
};

#endif // TVPARSER_H
