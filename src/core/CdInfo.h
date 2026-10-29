#ifndef CDRIPPER_CDINFO_H_
#define CDRIPPER_CDINFO_H_

#include <sys/types.h>

#include <cdio/cdio.h>
#include <QHash>
#include <QDebug>

#include "TrackInfo.h"

class CdInfo {
public:
    CdInfo();

    bool isCdAvailable() const;

    int getFirstTrack() const;
    int getTotalTracks() const;
    TrackInfo getTrack(int track) const;

    void refresh(CdIo_t* driver);

private:
    discmode_t m_discMode;
    int m_firstTrack;
    QHash<int, TrackInfo> m_tracks;
};

#endif 
