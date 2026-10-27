#include <sys/types.h>

#include <cdio/cdio.h>
#include <QString>
#include <QStringList>

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

    bool isDriverAvailable();

    QString getDriverName();
    QString getDefaultDeviceName();
    QStringList getDeviceNames();

    void refresh();

private:
    std::unique_ptr<CdIo_t, CdIoDeleterFunctor> m_driver;
};
