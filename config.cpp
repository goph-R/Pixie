#include "config.h"

Config::Config() {
    imageExtensions = QStringList() << "jpg" << "jpeg" << "gif" << "png" << "bmp";
}

const QStringList Config::getImageExtensions() {
    return imageExtensions;
}
