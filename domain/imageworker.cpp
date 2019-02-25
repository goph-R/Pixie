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

    // load the image row by row  (buffer is 16MB)
    int rowHeight = static_cast<int>((2048.0f / size.width()) * 2048.0f);
    auto steps = size.height() / rowHeight;
    QFile file(path);
    file.open(QFile::ReadOnly);
    for (int row = 0; row <= steps && !isLoadingStopped(); ++row) {
        file.seek(0);
        // Why we need always recreate the reader ... ? (Tested, without it doesn't work.)
        QImageReader r(&file);
        loadRow(path, &r, size, row, rowHeight);
    }
    file.close();
    emit done(path);
}

void ImageWorker::loadRow(QString path, QImageReader *reader, const QSize size, int row, int rowHeight) {
    int y1 = row * rowHeight;
    int y2 = y1 + rowHeight;
    if (y2 > size.height()) {
        y2 = size.height();
    }
    int h = y2 - y1;
    if (h <= 0) {
        return;
    }
    QRect rect(0, y1, size.width(), h);
    reader->setClipRect(rect);
    auto part = reader->read();
    emit loaded(path, rect, part);
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
