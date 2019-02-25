#include "domain/thumbnailworker.h"

#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include "domain/thumbnaildatabase.h"
#include "domain/config.h"
#include "domain/file.h"

#include <QDebug>

ThumbnailWorker::ThumbnailWorker(QObject* receiver, File* file, Config* config) : QRunnable() {
    setAutoDelete(true);
    this->receiver = receiver;
    path = file->getPath();
    folder = file->isFolder();
    size = config->getThumbnailSize();
    if (folder) {
        size = static_cast<int>((static_cast<float>(size) * 0.68f));
    }
    imageNameFilters = config->getImageFileNameFilters();
}

void ThumbnailWorker::run() {
    QString imagePath = path;    
    if (folder) {
        imagePath = getFirstImagePath();
        if (imagePath == "") {
            QMetaObject::invokeMethod(receiver, "emptySlot", Qt::QueuedConnection);
            return;
        }
    }
    if (createThumbnail(imagePath)) {
        int format = imagePath.toLower().endsWith(".png") ? ThumbnailDatabase::FORMAT_PNG : ThumbnailDatabase::FORMAT_JPEG;
        QMetaObject::invokeMethod(receiver, "doneSlot", Qt::QueuedConnection, Q_ARG(QString, path), Q_ARG(QImage, image), Q_ARG(int, format));
    } else {
        QMetaObject::invokeMethod(receiver, "errorSlot", Qt::QueuedConnection, Q_ARG(QString, path));
    }
}

QString ThumbnailWorker::getFirstImagePath() {
    QDir dir(path);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(imageNameFilters);
    dir.setSorting(QDir::Name);
    auto entries = dir.entryInfoList();
    return entries.size() ? entries.at(0).absoluteFilePath() : "";
}

bool ThumbnailWorker::createThumbnail(QString path) {
    QImageReader reader(path);
    if (!reader.canRead()) {
        return false;
    }
    auto imageSize = reader.size();
    float fastSize = size * 4;
    QImage fastScaled;
    if (imageSize.width() < fastSize || imageSize.height() < fastSize) {
        fastScaled = reader.read();
        image = fastScaled.scaled(size, size, Qt::KeepAspectRatio);
        return true;
    }
    if (!reader.supportsOption(QImageIOHandler::ScaledSize)) {
        QImage original = reader.read();
        fastScaled = original.scaled(size * 4, size * 4, Qt::KeepAspectRatio);
    } else {
        float w = imageSize.width();
        float h = imageSize.height();
        float nw = fastSize;
        float nh = fastSize;
        if (w > h) {
            nh = h/w * fastSize;
        } else {
            nw = w/h * fastSize;
        }
        reader.setScaledSize(QSize(static_cast<int>(nw), static_cast<int>(nh)));
        fastScaled = reader.read();
    }
    image = fastScaled.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return true;
}
