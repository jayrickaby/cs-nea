#include "Driver.h"

Driver::Driver()
    {};

bool Driver::isDriverAvailable() const {
    return m_cdio.get();
}

bool Driver::readSectorsToBuffer(lsn_t begin, lsn_t end, QIODevice* output) {
    if (!output || !output->isOpen() || !output->isWritable()) {
        qDebug() << "Output device is invalid!";
        return false;
    }
    
    lsn_t lastLsn {cdio_get_disc_last_lsn(m_cdio.get())};

    // Invalid bounds
    if (lastLsn == CDIO_INVALID_LSN || lastLsn <= 0) {
        qDebug() << "CD bounds are invalid!";
        return false;
    }

    if (begin > end) {
        qDebug() << "Input bounds are invalid!";
    }

    // Out of bounds
    if (end > lastLsn || end <= 0) {
        qDebug() << "Input upper LSN Bound is invalid!";
        return false;
    }

    if (begin > lastLsn || begin <=0) {
        qDebug() << "Input lower LSN bound is invalid!";
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
            qDebug() << "Error:" << error;
            free(error);
        }
        if (message) {
            qDebug() << "Message:" << message;
            free(message);
        }
        if (!raw) {
            qDebug() << "No data was retrieved!";
            continue;
        }

        // convert to actual characters
        qint64 written {output->write(reinterpret_cast<const char*>(raw), CDIO_CD_FRAMESIZE_RAW)};
        if (written != CDIO_CD_FRAMESIZE_RAW) {
            qDebug() << "Data written to output device does not match raw CD framesize!";
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

void Driver::refresh() {
    // Safely reset
    m_drive.reset();
    m_cdio.reset();

    // Get audio-only devices
    char **devices {cdio_get_devices_with_cap(nullptr, CDIO_FS_AUDIO, false)};

    if (!devices) {
        return;
    }

    m_cdio.reset(cdio_open(*devices, DRIVER_UNKNOWN));

    // Free up memory to prevent memory leak
    cdio_free_device_list(devices);

    if (!m_cdio) {
        return;
    }

    m_drive.reset(cdio_cddap_identify_cdio(m_cdio.get(), CDDA_MESSAGE_PRINTIT, nullptr));

    if (!m_drive) {
        return;
    }
    
    if (cdda_open(m_drive.get()) != 0) {
        qDebug() << "Unable to open disc!";
        // TODO: make refresh method a bool so that this and the returns can return false instead
        throw -5921512;
    }

    m_cdInfo.refresh(m_cdio.get());
}
