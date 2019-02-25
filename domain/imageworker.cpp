#include "imageworker.h"

#include <QImageReader>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

ImageWorker::ImageWorker() : QObject() {
}

void ImageWorker::load(QString path) {
    clearLoadingStop();
    QImageReader reader(path);
    if (!reader.supportsOption(QImageIOHandler::ClipRect)) {
        QImage image(path);
        emit loaded(image);
    } else {
        auto size = reader.size();
        auto steps = size.height() / 2048;
        for (int row = 0; row <= steps && !isLoadingStopped(); ++row) {
            loadRow(path, size, row);
        }
    }
}

void ImageWorker::loadRow(const QString path, const QSize size, int row) {
    int y1 = row * 2048;
    int y2 = y1 + 2048;
    if (y2 > size.height()) {
        y2 = size.height();
    }
    int h = y2 - y1;
    if (h <= 0) {
        return;
    }
    QImageReader reader(path);
    QRect r(0, y1, size.width(), h);
    reader.setClipRect(r);
    auto part = reader.read();
    emit partLoaded(r, part);
}

void ImageWorker::stopLoading() {
    stopMutex.lock();
    stop = true;
    stopMutex.unlock();
}

void ImageWorker::clearLoadingStop() {
    stopMutex.lock();
    stop = false;
    stopMutex.unlock();
}

bool ImageWorker::isLoadingStopped() {
    stopMutex.lock();
    bool result = stop;
    stopMutex.unlock();
    return result;
}
