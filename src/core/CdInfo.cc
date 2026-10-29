#include "CdInfo.h" 

CdInfo::CdInfo()
  : m_discMode(CDIO_DISC_MODE_NO_INFO),
    m_firstTrack(0)
    {};

bool CdInfo::isCdAvailable() const {
    return m_discMode == CDIO_DISC_MODE_CD_DA;
}

int CdInfo::getFirstTrack() const {
    return m_firstTrack;
}

int CdInfo::getTotalTracks() const {
    return m_tracks.size();
}

TrackInfo CdInfo::getTrack(int track) const {
    if (m_tracks.contains(track)) {
        return m_tracks[track];
    }
    return {};
}

void CdInfo::refresh(CdIo_t* driver) {
    m_discMode = CDIO_DISC_MODE_NO_INFO;
    m_firstTrack = 0;
    m_tracks.clear();

    if (!driver) {
        return;
    }

    m_discMode = cdio_get_discmode(driver);
    
    // Check whether CD can be read before trying to read
    if (!isCdAvailable()) {
        return;
    }

    m_firstTrack = cdio_get_first_track_num(driver);
    int totalTracksNumber = cdio_get_num_tracks(driver); 

    if (totalTracksNumber <= 0) {
        return;
    }

    m_tracks.reserve(totalTracksNumber);

    for (int i = 0; i < totalTracksNumber; i++) {
        int num {i + m_firstTrack};

        TrackInfo track {
            .lsn = cdio_get_track_lsn(driver, num),
            .number = num,
            .sectors = cdio_get_track_sec_count(driver, num)
        };

        m_tracks[num] = track;
    }
}
