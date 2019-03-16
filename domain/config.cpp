#include "domain/config.h"
#include <QImageReader>
#include <QDir>
#include <QDebug>
#include <QThreadPool>
#include <QCoreApplication>

QString Config::USER_FOLDER = ".pixie";

Config::Config() {
    thumbnailSize = 192;
    theme = "dark";
    setUpImageExtensions();
    setUpUserFolder();
}

bool Config::useBackslash() {
#ifdef __linux__
    return false;
#else
    return true;
#endif
}

void Config::setUpImageExtensions() {
    foreach (auto format, QImageReader::supportedImageFormats()) {
        imageExtensions << QString(format);
    }
    foreach (auto extension, imageExtensions) {
        imageFileNameFilters.append("*." + extension);
    }
}

const QString Config::getUserFolder() {
    auto result = QDir::homePath() + "/" + Config::USER_FOLDER + "/";
    return result;
}

const QString Config::getAppFolder() {
    return QCoreApplication::applicationDirPath() + "/";
}

void Config::setUpUserFolder() {
    QString homePath = QDir::homePath();
    QDir dir(getUserFolder());
    if (!dir.exists()) {
        QDir homeDir(homePath);
        homeDir.mkdir(Config::USER_FOLDER);
    }
    thumbnailsPath = cacheFolder + "thumbnails.s3db";
    QFile file(thumbnailsPath);
    if (!file.exists()) {
        QFile::copy(":/database/thumbnails.s3db", thumbnailsPath);
        file.setPermissions(QFile::ReadUser | QFile::WriteUser);
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

const QString Config::getThumbnailsPath() {
    return thumbnailsPath;
}

const QString Config::getTheme() {
    return theme;
}
