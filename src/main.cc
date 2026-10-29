#include <QDebug>

#include "core/Driver.h"
#include "core/TrackInfo.h"

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
    
    CdInfo cdInfo {driver.getCdInfo()};

    if (!cdInfo.isCdAvailable()) {
        qDebug() << "CD is not available.";
        return -1;
    }

    qDebug() << "CD First Track:" << cdInfo.getFirstTrack();
    qDebug() << "CD Total Tracks:" << cdInfo.getTotalTracks();

    for (int i = 0; i < cdInfo.getTotalTracks(); i++) {
        int num {i + cdInfo.getFirstTrack()};

        TrackInfo track {cdInfo.getTrack(num)};

        qDebug() << "Track:" << track.number << ", LSN:" << track.lsn << ", Sectors:" << track.sectors;
    }

    return 0;
}
