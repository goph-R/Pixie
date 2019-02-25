#include "imageworker.h"

#include <QImageReader>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include "domain/imagerowworker.h"

ImageWorker::ImageWorker() : QObject() {
    threadPool = QThreadPool::globalInstance();
}

ImageWorker::~ImageWorker() {
}

void ImageWorker::load(QString path) {
    if (path != getPathToLoad()) {
        return;
    }

    QImageReader reader(path);
    auto size = reader.size();
    QRect fullRect(0, 0, size.width(), size.height());

    // if clipping not supported load it in one
    if (!reader.supportsOption(QImageIOHandler::ClipRect)) {
        QImage image = reader.read();
        emit loaded(path, fullRect, image);
        emit done(path);
        return;
    }

    // if scaling supported send a very rough preview (the width is 100 big pixels)
    if (reader.supportsOption(QImageIOHandler::ScaledSize)) {
        int pixelSize = size.width() / 100;
        reader.setScaledSize(size / pixelSize);
        emit loaded(path, fullRect, reader.read());
    }

    if (path != getPathToLoad()) {
        return;
    }

    // load the image row by row  (buffer is 16MB)
    int rowHeight = static_cast<int>((2048.0f / size.width()) * 2048.0f);
    auto steps = size.height() / rowHeight;

    active = 0;
    for (int row = 0; row <= steps; ++row) {
        auto runnable = new ImageRowWorker(this, path, size, row, rowHeight);
        threadPool->start(runnable, QThread::HighPriority);
        ++active;
    }
}

void ImageWorker::loadedSlot(const QString path, const QRect rect, const QImage image) {
    if (path != getPathToLoad()) {
        return;
    }
    emit loaded(path, rect, image);
    --active;
    if (!active) {
        emit done(path);
    }
}

void ImageWorker::setPathToLoad(QString path) {
    pathToLoadMutex.lock();
    pathToLoad = path;
    pathToLoadMutex.unlock();
}

const QString ImageWorker::getPathToLoad() {
    pathToLoadMutex.lock();
    QString result = pathToLoad;
    pathToLoadMutex.unlock();
    return result;
}
