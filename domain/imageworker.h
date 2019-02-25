#ifndef IMAGEWORKER_H
#define IMAGEWORKER_H

#include <QObject>
#include <QImage>
#include <QRect>

class ImageWorker : public QObject
{
    Q_OBJECT

public:
    ImageWorker();

public slots:
    void load(QString path);

signals:
    void loaded(const QImage image);
    void partLoaded(const QRect, const QImage image);
};

#endif // IMAGEWORKER_H
