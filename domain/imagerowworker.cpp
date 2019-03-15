#include "imagerowworker.h"

#include <QRect>
#include <QImageReader>
#include <QDebug>
#include "domain/imageutils.h"
#include "domain/imageworker.h"

ImageRowWorker::ImageRowWorker(ImageWorker* worker, QString path, QSize size, int row, int rowHeight) : QRunnable() {
    this->worker = worker;
    this->path = path;
    this->size = size;
    this->row = row;
    this->rowHeight = rowHeight;
}

void ImageRowWorker::run() {
    workerMutex.lock();
    if (path != worker->getPathToLoad()) {
        workerMutex.unlock();
        return;
    }
    workerMutex.unlock();
    QImageReader reader(path);
    int y1 = row * rowHeight;
    int y2 = y1 + rowHeight;
    if (y2 > size.height()) {
        y2 = size.height();
    }
    int h = y2 - y1;
    QRect rect(0, y1, size.width(), h);
    reader.setClipRect(rect);
    auto image = reader.read();
    if (ImageUtils::needsTransform(&reader)) {
        image = ImageUtils::getTransformedImage(&reader, image);
        rect = ImageUtils::getTransformedRect(&reader, size, rect);
    }
    QMetaObject::invokeMethod(worker, "loadedSlot", Qt::QueuedConnection, Q_ARG(QString, path), Q_ARG(QRect, rect), Q_ARG(QImage, image));
}
