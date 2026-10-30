#ifndef CDRIPPER_DRIVER_H_
#define CDRIPPER_DRIVER_H_

#include <sys/types.h>

#include <cdio/cdio.h>
#include <cdio/cd_types.h>
#include <cdio/paranoia/cdda.h>
#include <QString>
#include <QStringList>

#include "CdInfo.h"

struct CdIoDeleterFunctor {
    void operator()(CdIo_t* p) {
        if (p) {
            // Free memory to prevent memory leak
            cdio_destroy(p);
        }
    }
};

struct CdRomDriveDeleterFunctor {
    void operator()(cdrom_drive_t* p) {
        if (p) {
            // Free memory to prevent memory leak
            // Don't free cdio so that seg fault doesn't happen
            cdio_cddap_close_no_free_cdio(p);
        }
    }
};

class Driver {
public:
    Driver();

    bool isDriverAvailable() const;

    CdInfo getCdInfo() const;

    QString getDriverName() const;
    QString getDefaultDeviceName() const;
    QStringList getDeviceNames() const;

    void refresh();

private:
    std::unique_ptr<CdIo_t, CdIoDeleterFunctor> m_cdio;
    std::unique_ptr<cdrom_drive_t, CdRomDriveDeleterFunctor> m_drive;
    CdInfo m_cdInfo;
};

#endif 
