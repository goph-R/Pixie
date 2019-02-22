#include "config.h"
#include <QImageReader>
#include <QDir>
#include <QDebug>

Config::Config() {
    thumbnailSize = 192;
    setUpImageExtensions();
    setUpCacheFolder();
}

void Config::setUpImageExtensions() {
    foreach (auto format, QImageReader::supportedImageFormats()) {
        imageExtensions << QString(format);
    }
    foreach (auto extension, imageExtensions) {
        imageFileNameFilters.append("*." + extension);
    }
}

void Config::setUpCacheFolder() {
    QString homePath = QDir::homePath();
    cacheFolder = homePath + "/.pixie/";
    QDir dir(cacheFolder);
    if (!dir.exists()) {
        dir.mkdir(".");
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

const QString Config::getCacheFolder() {
    return cacheFolder;
}
