#ifndef THUMBNAILFACTORY_H
#define THUMBNAILFACTORY_H

#include <QObject>
#include <QThreadPool>
#include <QImage>
#include "file.h"
#include "thumbnailrunner.h"
#include "config.h"

class ThumbnailQueue : public QObject
{
    Q_OBJECT

public:
    ThumbnailQueue(Config* config);
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
    Config* config;
    int retries;
    bool startNext();
};

#endif // THUMBNAILFACTORY_H
