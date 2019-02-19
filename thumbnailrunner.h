#ifndef THUMBNAILRUNNABLE_H
#define THUMBNAILRUNNABLE_H

#include <QRunnable>
#include <QString>
#include "thumbnailworker.h"

class ThumbnailRunner : public QRunnable
{
public:
    ThumbnailRunner(QString path);
    virtual ~ThumbnailRunner() override;
    virtual void run() override;
    ThumbnailWorker* getWorker();

private:
    ThumbnailWorker* worker;
};

#endif // THUMBNAILRUNNABLE_H
