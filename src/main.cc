#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <cdio/paranoia/cdda.h>

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

    QByteArray data;
    QBuffer buffer(&data);

    if (!buffer.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open buffer for writing.";
            return -1;
    }

    int lsnRange {30};
    qDebug() << "~ Reading first" << lsnRange << "LSN of track 3 ~";

    lsn_t begin {cdInfo.getTrack(3).lsn};

    if (!driver.readSectorsToBuffer(begin, begin + lsnRange, &buffer)) {
        qDebug () << "Failed to read.";
        return -1;
    }

    qDebug() << "Read" << data.size() << "bytes";

    return 0;
}
