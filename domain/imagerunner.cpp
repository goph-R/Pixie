#include "imagerunner.h"

#include <QRect>
#include <QImageReader>

ImageRunner::ImageRunner(QObject *receiver, QString path, QSize size, int row, int rowHeight) : QRunnable()
{
    this->receiver = receiver;
    this->path = path;
    this->size = size;
    this->row = row;
    this->rowHeight = rowHeight;
}

void ImageRunner::run() {
    QImageReader reader(path);
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
    reader.setClipRect(rect);
    auto part = reader.read();
    QMetaObject::invokeMethod(receiver, "imageLoaded", Qt::QueuedConnection, Q_ARG(QString, path), Q_ARG(QRect, rect), Q_ARG(QImage, part));
}
