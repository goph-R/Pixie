#include "thumbnailrunner.h"

#include <QDebug>

ThumbnailRunner::ThumbnailRunner(File* file, Config* config) : QRunnable() {
    setAutoDelete(true);
    worker = new ThumbnailWorker(nullptr, file, config);
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

