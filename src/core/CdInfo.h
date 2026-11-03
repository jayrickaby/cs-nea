#ifndef CDRIPPER_CDINFO_H_
#define CDRIPPER_CDINFO_H_

#include <sys/types.h>

#include <cdio/cdio.h>
#include <QDebug>
#include <QHash>
#include <QLoggingCategory>

#include "TrackInfo.h"

Q_DECLARE_LOGGING_CATEGORY(cdInfo)

class CdInfo {
public:
    CdInfo();

    bool isCdAvailable() const;
    bool refresh(CdIo_t* driver);

    int getFirstTrack() const;
    int getTotalTracks() const;
    TrackInfo getTrack(int track) const;

private:
    discmode_t m_discMode;
    int m_firstTrack;
    QHash<int, TrackInfo> m_tracks;
};

#endif 
