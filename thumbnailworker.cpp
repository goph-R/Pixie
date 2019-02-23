#include "thumbnailworker.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include "thumbnaildatabase.h"

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
    auto original = QImage(path);
    if (original.isNull()) {
        return false;
    }
    auto fastScaled = original.scaled(size * 4, size * 4, Qt::KeepAspectRatio);
    image = fastScaled.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return true;
}
