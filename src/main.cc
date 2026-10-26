#include <iostream>
#include <cdio/version.h>
#include <QtVersion>

int main() {
    std::cout << "Current libcdio version is: " << CDIO_VERSION << '\n';
    std::cout << "Current Qt version is: " << QT_VERSION_STR << '\n'

    return 0;
}
