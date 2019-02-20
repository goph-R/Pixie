#ifndef THUMBNAILRUNNABLE_H
#define THUMBNAILRUNNABLE_H

#include <QRunnable>
#include <QString>
#include "thumbnailworker.h"
#include "config.h"
#include "file.h"

class ThumbnailRunner : public QRunnable
{
public:
    ThumbnailRunner(File* file, Config* config);
    virtual ~ThumbnailRunner() override;
    virtual void run() override;
    ThumbnailWorker* getWorker();

private:
    ThumbnailWorker* worker;
};

#endif // THUMBNAILRUNNABLE_H
