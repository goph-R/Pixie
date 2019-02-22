#include "config.h"
#include <QImageReader>
#include <QDebug>

Config::Config() {
    thumbnailSize = 192;
    foreach (auto format, QImageReader::supportedImageFormats()) {
        imageExtensions << QString(format);
    }
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
