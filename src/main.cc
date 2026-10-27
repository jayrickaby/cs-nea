#include <QDebug>

#include "core/Driver.h"

int main() {
    Driver driver;
    driver.refresh();
    
    if (!driver.isDriverAvailable()) {
        qDebug() << "CD Driver is not available";
        return -1;
    }

    qDebug() << "Name:" << driver.getDriverName();
    qDebug() << "Default device:" << driver.getDefaultDeviceName();
    qDebug() << "Devices:" << driver.getDeviceNames();

    return 0;
}
