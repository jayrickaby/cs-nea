#include "Driver.h"

Driver::Driver()
    {};

bool Driver::isDriverAvailable() const {
    return (m_driver.get());
}

CdInfo Driver::getCdInfo() const {
    return m_cdInfo;
}

QString Driver::getDriverName() const {
    // Check that invalid memory isn't being accessed
    if (m_driver.get()) {
        return cdio_get_driver_name(m_driver.get());
    }

    return {};
}

QString Driver::getDefaultDeviceName() const {
    // Check that invalid memory isn't being accessed
    if (m_driver.get()) {
        return cdio_get_default_device(m_driver.get());
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
    m_driver.reset(cdio_open(nullptr, DRIVER_DEVICE));
    m_cdInfo.refresh(m_driver.get());
}
