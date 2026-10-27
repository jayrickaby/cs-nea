#include "Driver.h"

Driver::Driver()
    {};

bool Driver::isDriverAvailable() {
    return (m_driver.get());
}

QString Driver::getDriverName() {
    // Check that invalid memory isn't being accessed
    if (m_driver.get()) {
        return cdio_get_driver_name(m_driver.get());
    }

    return {};
}

QString Driver::getDefaultDeviceName() {
    // Check that invalid memory isn't being accessed
    if (m_driver.get()) {
        return cdio_get_default_device(m_driver.get());
    }

    return {};
}

QStringList Driver::getDeviceNames() {
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
}
