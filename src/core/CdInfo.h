#ifndef CDRIPPER_CDINFO_H_
#define CDRIPPER_CDINFO_H_

#include <sys/types.h>

#include <cdio/cdio.h>
#include <QHash>
#include <QDebug>

class CdInfo {
public:
    CdInfo();

    bool isCdAvailable() const;

    int getFirstTrack() const;
    int getTotalTracks() const;
    int getTrackLSN(int track) const;

    void refresh(CdIo_t* driver);

private:
    discmode_t m_discMode;
    int m_firstTrack;
    int m_totalTracks;
    QHash<int, int> m_trackLSNs;
};

#endif 
