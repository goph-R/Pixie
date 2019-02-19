#ifndef THUMBNAILFACTORY_H
#define THUMBNAILFACTORY_H

#include <QObject>
#include <QThreadPool>
#include <QImage>
#include <QTimer>
#include "file.h"
#include "thumbnailrunner.h"

class ThumbnailQueue : public QObject
{
    Q_OBJECT

public:
    ThumbnailQueue();
    ~ThumbnailQueue();
    void add(File* file);
    void start();
    void clear();

public slots:
    void doneSlot(QString path, QImage image);

signals:
    void done(QString path, QImage image);

private:
    QList<ThumbnailRunner*> queue;
    QThreadPool threadPool;
    QTimer timer;
    int retries;
    bool startNext();
};

#endif // THUMBNAILFACTORY_H
