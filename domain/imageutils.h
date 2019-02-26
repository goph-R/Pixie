#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QSize>
#include <QImageReader>
#include <QRect>

class ImageUtils
{
public:
    static QSize getTransformedSize(QImageReader* reader, const QSize size);
    static QRect getTransformedRect(QImageReader* reader, const QSize size, const QRect rect);
    static bool needsTransform(QImageReader* reader);
    static QImage getTransformedImage(QImageReader* reader, const QImage image);
};

#endif // IMAGEUTILS_H
