#include "config.h"

Config::Config() {
    thumbnailSize = 192;
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

int Config::getThumbnailSize() {
    return thumbnailSize;
}
