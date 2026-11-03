#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <cdio/logging.h>
#include <cdio/paranoia/cdda.h>

#include "core/Driver.h"
#include "core/TrackInfo.h"

Q_DECLARE_LOGGING_CATEGORY(cdio)
Q_LOGGING_CATEGORY(cdio, "cdio")

static void qCdIoLogHandler(cdio_log_level_t level, const char message[]) {
    switch (level) {
        case CDIO_LOG_DEBUG:
              qCDebug(cdio) << message;
              break;
        case CDIO_LOG_INFO:
              qCInfo(cdio) << message;
              break;
        case CDIO_LOG_WARN:
              qCWarning(cdio) << message;
              break;
        case CDIO_LOG_ERROR:
              qCCritical(cdio) << message;
              break;
        case CDIO_LOG_ASSERT:
              qCFatal(cdio) << message;
              break;
        default:
              qCDebug(cdio) << message;
              break;
    }
}

int main() {
    qSetMessagePattern("[%{type}][%{category}]: %{message}");

    cdio_log_set_handler(qCdIoLogHandler);

    Driver driver;
    if (!driver.refresh()) {
        qFatal() << "Failed to refresh driver!";
        return -1;
    }
    
    if (!driver.isDriverAvailable()) {
        qFatal() << "CD Driver is not available!";
        return -1;
    }

    qDebug() << "Name:" << driver.getDriverName();
    qDebug() << "Default device:" << driver.getDefaultDeviceName();
    qDebug() << "Devices:" << driver.getDeviceNames();
    
    CdInfo cdInfo {driver.getCdInfo()};

    if (!cdInfo.isCdAvailable()) {
        qFatal() << "CD is not available!";
        return -1;
    }

    qDebug() << "CD First Track:" << cdInfo.getFirstTrack();
    qDebug() << "CD Total Tracks:" << cdInfo.getTotalTracks();

    for (int i = 0; i < cdInfo.getTotalTracks(); i++) {
        int num {i + cdInfo.getFirstTrack()};

        TrackInfo track {cdInfo.getTrack(num)};

        qDebug() << "Track:" << track.number << ", LSN:" << track.lsn << ", Sectors:" << track.sectors;
    }
    QFile file("track3.raw");

    if (!file.open(QIODevice::WriteOnly)) {
        qFatal() << "Failed to open file for writing!";
        return -1;
    }
    
    TrackInfo track {cdInfo.getTrack(3)};

    qDebug() << "~ Reading track 3 ~";

    if (!driver.readSectorsToBuffer(track.lsn, track.lsn + track.sectors, &file)) {
        qFatal() << "Failed to read CD!";
        return -1;
    }

    qDebug() << "Read" << file.size() << "bytes";

    file.close();

    return 0;
}
