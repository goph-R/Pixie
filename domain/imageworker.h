#ifndef IMAGEWORKER_H
#define IMAGEWORKER_H

#include <QObject>
#include <QImage>
#include <QRect>
#include <QMutex>
#include <QFile>
#include <QImageReader>

class ImageWorker : public QObject
{
    Q_OBJECT

public:
    ImageWorker();

public slots:
    void load(QString path);
    void stopLoading();

signals:
    void loaded(const QString path, const QRect, const QImage image);
    void done(const QString path);

private:
    QMutex stopMutex;
    bool stop;
    bool isLoadingStopped();
    void clearLoadingStop();
    void loadRow(QString path, QImageReader* reader, const QSize size, int row, int rowHeight);
};

#endif // IMAGEWORKER_H
