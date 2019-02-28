#include "imageutils.h"

#include <QImageIOHandler>
#include <QDebug>

QSize ImageUtils::getTransformedSize(QImageReader* reader, const QSize size) {
    QSize result = size;
#if QT_VERSION >= 0x050500
    if (reader->transformation() & QImageIOHandler::TransformationRotate90) {
        result.setWidth(size.height());
        result.setHeight(size.width());
    }
#endif
    return result;
}

QRect ImageUtils::getTransformedRect(QImageReader* reader, const QSize size, const QRect rect) {
    QRect result = rect;
#if QT_VERSION >= 0x050500
    auto t = reader->transformation();
    if (t & QImageIOHandler::TransformationMirror) {
        result.setX(size.width() - rect.x() - rect.width());
    }
    if (t & QImageIOHandler::TransformationFlip) {
        result.setY(size.height() - rect.y() - rect.height());
    }
    if (t & QImageIOHandler::TransformationRotate90) {
        result.setX(rect.y());
        result.setY(rect.x());
        if ((t & QImageIOHandler::TransformationMirror) == 0) {
            result.setX(size.height() - rect.y() - rect.height());
        }
        if ((t & QImageIOHandler::TransformationFlip) == 0) {
            result.setY(size.width() - rect.x() - rect.width());
        }
        result.setWidth(rect.height());
        result.setHeight(rect.width());
    }
#endif
    return result;
}

bool ImageUtils::needsTransform(QImageReader *reader) {
    bool result = false;
#if QT_VERSION >= 0x050500
    if (reader->transformation()) {
        result = true;
    }
#endif
    return result;
}

QImage ImageUtils::getTransformedImage(QImageReader *reader, const QImage image) {
    QImage result = image;
#if QT_VERSION >= 0x050500
    auto t = reader->transformation();
    if (!t) {
        return result;
    }
    result = result.mirrored(t & QImageIOHandler::TransformationMirror, t & QImageIOHandler::TransformationFlip);
    if (t & QImageIOHandler::TransformationRotate90) {
        QMatrix matrix;
        result = result.transformed(matrix.rotate(90));
    }
#endif
    return result;
}
