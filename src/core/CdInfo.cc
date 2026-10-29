#include "CdInfo.h" 

CdInfo::CdInfo()
  : m_discMode(CDIO_DISC_MODE_NO_INFO),
    m_firstTrack(0),
    m_totalTracks(0)
    {};

bool CdInfo::isCdAvailable() const {
    return m_discMode == CDIO_DISC_MODE_CD_DA;
}

int CdInfo::getFirstTrack() const {
    return m_firstTrack;
}

int CdInfo::getTotalTracks() const {
    return m_totalTracks;
}

int CdInfo::getTrackLSN(int track) const {
    if (m_trackLSNs.contains(track)) {
        return m_trackLSNs[track];
    }

    return -1;
}

void CdInfo::refresh(CdIo_t* driver) {
    m_firstTrack = 0;
    m_totalTracks = 0;
    m_trackLSNs.clear();
    m_discMode = CDIO_DISC_MODE_NO_INFO;

    if (!driver) {
        return;
    }

    // Check whether CD can be read before trying to read
    m_discMode = cdio_get_discmode(driver);
    
    if (!isCdAvailable()) {
        return;
    }

    m_firstTrack = cdio_get_first_track_num(driver);
    m_totalTracks = cdio_get_num_tracks(driver); 

    if (m_totalTracks <= 0) {
        return;
    }

    for (int i = 0; i < m_totalTracks; i++) {
        int track {i + m_firstTrack};

        m_trackLSNs[track] = cdio_get_track_lsn(driver, track);
    }
}
