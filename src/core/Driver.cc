#include "Driver.h"

Driver::Driver()
    {};

bool Driver::isDriverAvailable() const {
    return m_cdio.get();
}

bool Driver::readSectorsToBuffer(lsn_t begin, lsn_t end, QIODevice* output) {
    qDebug() << "Attempting to read sectors:" << begin << "to" << end << "to buffer";

    if (!output || !output->isOpen() || !output->isWritable()) {
        qCritical() << "Output device is invalid!";
        return false;
    }
    
    lsn_t lastLsn {cdio_get_disc_last_lsn(m_cdio.get())};

    // Invalid bounds
    if (lastLsn == CDIO_INVALID_LSN || lastLsn < 0) {
        qCritical() << "CD bounds are invalid!";
        return false;
    }

    if (begin > end) {
        qCritical() << "Input bounds are invalid!";
        return false;
    }

    // Out of bounds
    if (end > lastLsn || end < 0) {
        qCritical() << "Input upper LSN Bound is invalid!";
        return false;
    }

    if (begin > lastLsn || begin < 0) {
        qCritical() << "Input lower LSN bound is invalid!";
        return false;
    }

    cdrom_paranoia_t *paranoia {paranoia_init(m_drive.get())};

    // skip over failed attempts
    paranoia_modeset(paranoia, PARANOIA_MODE_FULL^PARANOIA_MODE_NEVERSKIP);
    
    cdda_verbose_set(m_drive.get(), CDDA_MESSAGE_PRINTIT, CDDA_MESSAGE_PRINTIT);
    
    lsn_t cursor;

    paranoia_seek(paranoia, begin, SEEK_SET);

    for (cursor = begin; cursor < end; cursor++) {
        qDebug() << "Reading LSN:" << cursor;
        int16_t *raw {paranoia_read(paranoia, nullptr)};
        char *error {cdda_errors(m_drive.get())};
        char *message {cdda_messages(m_drive.get())};

        if (error) {
            qWarning() << "Error:" << error;
            free(error);
        }
        if (message) {
            qWarning() << "Message:" << message;
            free(message);
        }
        if (!raw) {
            qWarning() << "No data was retrieved!";
            continue;
        }

        // convert to actual characters
        qint64 written {output->write(reinterpret_cast<const char*>(raw), CDIO_CD_FRAMESIZE_RAW)};
        if (written != CDIO_CD_FRAMESIZE_RAW) {
            qCritical() << "Data written to output device does not match raw CD framesize!";
            paranoia_free(paranoia);
            return false;
        }
    }

    paranoia_free(paranoia);

    return true;
}

CdInfo Driver::getCdInfo() const {
    return m_cdInfo;
}

QString Driver::getDriverName() const {
    // Check that invalid memory isn't being accessed
    if (m_cdio.get()) {
        return cdio_get_driver_name(m_cdio.get());
    }

    return {};
}

QString Driver::getDefaultDeviceName() const {
    // Check that invalid memory isn't being accessed
    if (m_cdio.get()) {
        return cdio_get_default_device(m_cdio.get());
    }

    return {};
}

QStringList Driver::getDeviceNames() const {
    char** drives {cdio_get_devices(DRIVER_DEVICE)};
    
    QStringList list;

    if (drives) {
        for (char **c {drives}; *c != nullptr; c++) {
            list.append(*c);
        }

        // Free memory to prevent memory leak
        cdio_free_device_list(drives);
    }

    return list;
}

bool Driver::refresh() {
    qDebug() << "Refreshing Driver...";
    // Safely reset
    m_drive.reset();
    m_cdio.reset();

    // Get audio-only devices
    char **devices {cdio_get_devices_with_cap(nullptr, CDIO_FS_AUDIO, false)};

    if (!devices) {
        qCritical() << "No audio devices found!";
        return false;
    }

    m_cdio.reset(cdio_open(*devices, DRIVER_UNKNOWN));

    // Free up memory to prevent memory leak
    cdio_free_device_list(devices);

    if (!m_cdio) {
        qCritical() << "Failed to open CdIo!";
        return false;
    }

    m_drive.reset(cdio_cddap_identify_cdio(m_cdio.get(), CDDA_MESSAGE_PRINTIT, nullptr));

    if (!m_drive) {
        qCritical() << "Failed to identify drive from CdIo!";
        return false;
    }
    
    if (cdda_open(m_drive.get()) != 0) {
        qCritical() << "Unable to open disc!";
        return false;
    }

    if (!m_cdInfo.refresh(m_cdio.get())) {
        qCritical() << "Failed to get CD Info!";
        return false;
    }

    qDebug() << "Successfully refreshed Driver.";
    return true;
}
