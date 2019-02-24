#include "imageworker.h"

ImageWorker::ImageWorker() : QObject() {
}

void ImageWorker::load(QString path) {
    // TODO: use QImageReader::setClipRect if possible and read it part by part
    QImage image(path);
    emit loaded(image);
}

