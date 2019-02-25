#include "imageworker.h"

#include <QImageReader>
#include <QFile>
#include <QDebug>

ImageWorker::ImageWorker() : QObject() {
}

void ImageWorker::load(QString path) {
    QImageReader reader(path);
    if (!reader.supportsOption(QImageIOHandler::ClipRect)) {
        QImage image(path);
        emit loaded(image);
        return;
    }
    auto size = reader.size();
    auto steps = size.height() / 2048;
    for (int i = 0; i <= steps; ++i) {
        QImageReader reader(path);
        int y1 = i * 2048;
        int y2 = y1 + 2048;
        if (y2 > size.height()) {
            y2 = size.height();
        }
        int h = y2 - y1;
        if (h) {
            QRect r(0, y1, size.width(), h);
            reader.setClipRect(r);
            auto part = reader.read();
            emit partLoaded(r, part);
        }
    }
}

