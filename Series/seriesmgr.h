#ifndef SERIESMGR_H
#define SERIESMGR_H

#include "series.h"
#include "singleton.h"

typedef QMap<QString, Series*> SeriesMap;

class SeriesMgr
{
    public:
        SeriesMgr() {}
        ~SeriesMgr() {}

        void Load(QString _setting = QString(), QString _error = QString());
        Series* GetSeries(QString name) const { return lSeries_m.contains(name) ? lSeries_m[name] : NULL; }
        const SeriesMap& GetMap() const { return lSeries_m; }

        Site GetSiteFromString(QString _s);
        void writeError(QString error);

    private:
        SeriesMap lSeries_m;

        QString fSeries_m;
        QString fError_m;
};

#define sSeries Singleton<SeriesMgr>::Instance()

#endif // SERIESMGR_H
