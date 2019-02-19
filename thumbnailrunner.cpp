#include "thumbnailrunner.h"

#include <QDebug>

ThumbnailRunner::ThumbnailRunner(QString path) : QRunnable() {
    setAutoDelete(true);
    worker = new ThumbnailWorker(path);
}

ThumbnailRunner::~ThumbnailRunner() {
    delete worker;
}

ThumbnailWorker* ThumbnailRunner::getWorker() {
    return worker;
}

void ThumbnailRunner::run() {
    worker->run();
}

