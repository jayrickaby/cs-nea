#include "Driver.h"

Q_LOGGING_CATEGORY(driver, "driver")
Q_LOGGING_CATEGORY(paranoia, "paranoia")

Driver::Driver()
    {};

bool Driver::isDriverAvailable() const {
    return m_cdio.get();
}

bool Driver::readSectorsToBuffer(lsn_t begin, lsn_t end, QIODevice* output) {
    qCDebug(driver) << "Attempting to read sectors:" << begin << "to" << end << "to buffer";

    if (!output || !output->isOpen() || !output->isWritable()) {
        qCCritical(driver) << "Output device is invalid!";
        return false;
    }
    
    lsn_t lastLsn {cdio_get_disc_last_lsn(m_cdio.get())};

    // Invalid bounds
    if (lastLsn == CDIO_INVALID_LSN || lastLsn < 0) {
        qCCritical(driver) << "CD bounds are invalid!";
        return false;
    }

    if (begin > end) {
        qCCritical(driver) << "Input bounds are invalid!";
        return false;
    }

    // Out of bounds
    if (end > lastLsn || end < 0) {
        qCCritical(driver) << "Input upper LSN Bound is invalid!";
        return false;
    }

    if (begin > lastLsn || begin < 0) {
        qCCritical(driver) << "Input lower LSN bound is invalid!";
        return false;
    }

    cdrom_paranoia_t *paranoia {paranoia_init(m_drive.get())};

    // skip over failed attempts
    paranoia_modeset(paranoia, PARANOIA_MODE_FULL^PARANOIA_MODE_NEVERSKIP);
    
    cdda_verbose_set(m_drive.get(), CDDA_MESSAGE_LOGIT, CDDA_MESSAGE_LOGIT);
    
    lsn_t cursor;

    paranoia_seek(paranoia, begin, SEEK_SET);

    for (cursor = begin; cursor < end; cursor++) {
        qCDebug(driver) << "Reading LSN:" << cursor;
        int16_t *raw {paranoia_read(paranoia, nullptr)};
        
        pollParanoiaLogs();

        if (!raw) {
            qCWarning(driver) << "No data was retrieved!";
            continue;
        }

        // convert to actual characters
        qint64 written {output->write(reinterpret_cast<const char*>(raw), CDIO_CD_FRAMESIZE_RAW)};
        if (written != CDIO_CD_FRAMESIZE_RAW) {
            qCCritical(driver) << "Data written to output device does not match raw CD framesize!";
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
    qCDebug(driver) << "Refreshing Driver...";
    // Safely reset
    m_drive.reset();
    m_cdio.reset();

    // Get audio-only devices
    char **devices {cdio_get_devices_with_cap(nullptr, CDIO_FS_AUDIO, false)};

    if (!devices) {
        qCCritical(driver) << "No audio devices found!";
        return false;
    }

    m_cdio.reset(cdio_open(*devices, DRIVER_UNKNOWN));

    // Free up memory to prevent memory leak
    cdio_free_device_list(devices);

    if (!m_cdio) {
        qCCritical(driver) << "Failed to open CdIo!";
        return false;
    }

    char* message {nullptr};
    m_drive.reset(cdio_cddap_identify_cdio(m_cdio.get(), CDDA_MESSAGE_LOGIT, &message));

    if (message) {
        qCDebug(paranoia) << message;
        cdio_cddap_free_messages(message);
    }

    if (!m_drive) {
        qCCritical(driver) << "Failed to identify drive from CdIo!";
        return false;
    }
    
    if (cdda_open(m_drive.get()) != 0) {
        qCCritical(driver) << "Unable to open disc!";
        return false;
    }

    if (!m_cdInfo.refresh(m_cdio.get())) {
        qCCritical(driver) << "Failed to get CD Info!";
        return false;
    }

    qCDebug(driver) << "Successfully refreshed Driver.";
    return true;
}

void Driver::pollParanoiaLogs() const {
    if (!m_drive.get()) {
        return;
    }

    char* error = cdda_errors(m_drive.get());
    if (error) {
        qCDebug(paranoia) << error;
        cdio_cddap_free_messages(error);
    }

    char* message = cdda_messages(m_drive.get());
    if (message) {
        qCWarning(paranoia) << error;
        cdio_cddap_free_messages(message);
    }
}
