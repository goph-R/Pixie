#ifndef IMAGEWORKER_H
#define IMAGEWORKER_H

#include <QObject>
#include <QImage>
#include <QRect>
#include <QMutex>

class ImageWorker : public QObject
{
    Q_OBJECT

public:
    ImageWorker();

public slots:
    void load(QString path);
    void stopLoading();

signals:
    void loaded(const QImage image);
    void partLoaded(const QRect, const QImage image);

private:
    QMutex stopMutex;
    bool stop;
};

#endif // IMAGEWORKER_H
