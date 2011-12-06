#ifndef BASICPARSER_H
#define BASICPARSER_H

#include <QObject>
#include "series.h"

class BasicParser : public QObject
{
    Q_OBJECT

    public:
        BasicParser(Series* _s, const QByteArray _c);
        ~BasicParser();
    protected:
        Series* series_m;
        const QByteArray content_m;
};

#endif // BASICPARSER_H
