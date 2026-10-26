#include <iostream>
#include <sys/types.h>

#include <cdio/cdio.h>
#include <QDebug>

bool isDriveConnected() {
    // Open any available CD Driver device
    CdIo_t *p_cdio {cdio_open(nullptr, DRIVER_DEVICE)};

    if (p_cdio) {
        qDebug() << "The selected driver is:" << cdio_get_driver_name(p_cdio) << '\n';
        qDebug() << "The default device for this driver is:" << cdio_get_default_device(p_cdio) << '\n';
        cdio_destroy(p_cdio);
        return true;
    }

    qDebug() << "There was an error trying to find a driver." << '\n';
    return false;

}

int main() {
    bool connected {isDriveConnected()};

    if (!connected) {
        return -1;
    }

    return 0;
}
