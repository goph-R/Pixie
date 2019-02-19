#include "thumbnailworker.h"

#include <QDebug>

ThumbnailWorker::ThumbnailWorker(QString path) : QObject() {
    this->path = path;
}

void ThumbnailWorker::run() {
    auto original = QImage(path);
    if (original.isNull()) {
        emit error(path);
        return;
    }
    auto fastScaled = original.scaled(512, 512, Qt::KeepAspectRatio);
    auto image = fastScaled.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    emit done(path, image);
}
