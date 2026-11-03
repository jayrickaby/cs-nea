#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <cdio/paranoia/cdda.h>

#include "core/Driver.h"
#include "core/TrackInfo.h"

int main() {
    qSetMessagePattern("[%{type}][%{category}]: %{message}");

    Driver driver;
    if (!driver.refresh()) {
        qDebug() << "Failed to refresh driver!";
        return -1;
    }
    
    if (!driver.isDriverAvailable()) {
        qDebug() << "CD Driver is not available!";
        return -1;
    }

    qDebug() << "Name:" << driver.getDriverName();
    qDebug() << "Default device:" << driver.getDefaultDeviceName();
    qDebug() << "Devices:" << driver.getDeviceNames();
    
    CdInfo cdInfo {driver.getCdInfo()};

    if (!cdInfo.isCdAvailable()) {
        qDebug() << "CD is not available!";
        return -1;
    }

    qDebug() << "CD First Track:" << cdInfo.getFirstTrack();
    qDebug() << "CD Total Tracks:" << cdInfo.getTotalTracks();

    for (int i = 0; i < cdInfo.getTotalTracks(); i++) {
        int num {i + cdInfo.getFirstTrack()};

        TrackInfo track {cdInfo.getTrack(num)};

        qDebug() << "Track:" << track.number << ", LSN:" << track.lsn << ", Sectors:" << track.sectors;
    }
    QBuffer buffer(&data);

    QFile file("track3.raw");

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open buffer for writing!";
            return -1;
    }
    
    TrackInfo track {cdInfo.getTrack(3)};

    qDebug() << "~ Reading track 3 ~";

    if (!driver.readSectorsToBuffer(track.lsn, track.lsn + track.sectors, &file)) {
        qDebug () << "Failed to read CD!";
        return -1;
    }

    qDebug() << "Read" << file.size() << "bytes";

    file.close();

    return 0;
}
