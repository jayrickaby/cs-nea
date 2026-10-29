#ifndef CDRIPPER_DRIVER_H_
#define CDRIPPER_DRIVER_H_

#include <sys/types.h>

#include <cdio/cdio.h>
#include <QString>
#include <QStringList>

#include "CdInfo.h"

struct CdIoDeleterFunctor {
    void operator()(CdIo_t* p){
        if (p) {
            // Free memory to prevent memory leak
            cdio_destroy(p);
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
    std::unique_ptr<CdIo_t, CdIoDeleterFunctor> m_driver;
    CdInfo m_cdInfo;
};

#endif 
