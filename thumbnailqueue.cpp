#include "thumbnailqueue.h"

#include <QDebug>

ThumbnailQueue::ThumbnailQueue(Config* config) : QObject() {
    active = 0;
    this->config = config;
}

ThumbnailQueue::~ThumbnailQueue() {
    clear();
    threadPool.waitForDone();
}

void ThumbnailQueue::add(File* file) {
    auto task = new ThumbnailRunner(file, config);
    queue.append(task);
}

void ThumbnailQueue::clear() {
    while (queue.size()) {
        auto task = queue.takeLast();
        delete task;
    }
}

void ThumbnailQueue::start() {
    int max = threadPool.maxThreadCount();
    while (queue.size() && active < max) {
        startNext();
    }
}

void ThumbnailQueue::startNext() {
    active++;
    auto task = queue.takeFirst();
    threadPool.start(task);
    auto worker = task->getWorker();
    connect(worker, &ThumbnailWorker::done, this, &ThumbnailQueue::doneSlot);
    connect(worker, &ThumbnailWorker::error, this, &ThumbnailQueue::errorSlot);
    connect(worker, &ThumbnailWorker::empty, this, &ThumbnailQueue::emptySlot);
}

void ThumbnailQueue::doneSlot(QString path, QImage image) {
    emit done(path, image);
    workerFinished();
}

void ThumbnailQueue::errorSlot(QString path) {
    emit error(path);
    workerFinished();
}

void ThumbnailQueue::emptySlot() {
    workerFinished();
}

void ThumbnailQueue::workerFinished() {
    active--;
    start();
}
