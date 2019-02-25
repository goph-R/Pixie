#include "imageworker.h"

#include <QImageReader>

ImageWorker::ImageWorker() : QObject() {
}

void ImageWorker::load(QString path) {
    // TODO: use QImageReader::setClipRect if possible and read it part by part
    QImageReader reader(path);
    auto size = reader.size();
    auto steps = size.height() / 2000;
    for (int i = 0; i <= steps; ++i) {
        QImageReader reader(path);
        int y1 = i * 2000;
        int y2 = y1 + 2000;
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

    //QImage image(path);
    //emit loaded(image);
}

