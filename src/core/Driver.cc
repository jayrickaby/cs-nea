#include "Driver.h"

Driver::Driver()
    {};

bool Driver::isDriverAvailable() const {
    return m_cdio.get();
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

    m_cdInfo.refresh(m_cdio.get());
}
