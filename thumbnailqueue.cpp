#include "thumbnailqueue.h"

#include <QDebug>

ThumbnailQueue::ThumbnailQueue() : QObject() {
    retries = 0;
}

ThumbnailQueue::~ThumbnailQueue() {
    clear();
    threadPool.waitForDone();
}

void ThumbnailQueue::add(File* file) {
    auto task = new ThumbnailRunner(file->getPath());
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
    while (queue.size() && threadPool.activeThreadCount() < max && retries < max) {
        if (!startNext()) {
            retries++;
        }
    }
    if (retries == max) {
        qDebug() << "ThumbnailQueue tried to start a task too many times";
    }
}

bool ThumbnailQueue::startNext() {
    auto task = queue.at(0);
    bool result = false;
    if (threadPool.tryStart(task)) {
        queue.takeFirst();
        auto worker = task->getWorker();
        connect(worker, &ThumbnailWorker::done, this, &ThumbnailQueue::doneSlot);
        result = true;
    }
    return result;
}

void ThumbnailQueue::doneSlot(QString path, QImage image) {
    emit done(path, image);
    start();
}
