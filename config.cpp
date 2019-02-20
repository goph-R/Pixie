#include "config.h"

Config::Config() {
    imageExtensions = QStringList() << "jpg" << "jpeg" << "gif" << "png" << "bmp";
    foreach (auto extension, imageExtensions) {
        imageFileNameFilters.append("*." + extension);
    }
}

const QStringList Config::getImageExtensions() {
    return imageExtensions;
}

const QStringList Config::getImageFileNameFilters() {
    return imageFileNameFilters;
}
