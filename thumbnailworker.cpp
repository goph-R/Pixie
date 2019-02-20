#include "thumbnailworker.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

ThumbnailWorker::ThumbnailWorker(File* file, Config* config) : QObject() {
    path = file->getPath();
    folder = file->isFolder();
    size = folder ? 88 : 128;
    imageNameFilters = config->getImageFileNameFilters();
}

void ThumbnailWorker::run() {
    QString imagePath = path;
    if (folder) {
        imagePath = getFirstImagePath();
        if (imagePath == "") {
            return;
        }
    }
    if (createThumbnail(imagePath)) {
        emit done(path, image);
    } else {
        emit error(path);
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
