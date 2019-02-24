#include "domain/thumbnailqueue.h"

#include "config.h"
#include "file.h"
#include "filemanager.h"
#include "thumbnailworker.h"
#include "thumbnaildatabase.h"

#include <QDebug>

ThumbnailQueue::ThumbnailQueue(Config* config, FileManager* fileManager) : QObject() {
    active = 0;
    this->config = config;
    this->fileManager = fileManager;
    createDatabaseThread();
}

ThumbnailQueue::~ThumbnailQueue() {
    clear();
    threadPool.waitForDone();
    databaseThread.quit();
    databaseThread.wait();
}

void ThumbnailQueue::createDatabaseThread() {
    auto database = new ThumbnailDatabase(config->getThumbnailsPath());
    database->moveToThread(&databaseThread);
    QObject::connect(this, SIGNAL(connectDatabase()), database, SLOT(connect()));
    QObject::connect(this, SIGNAL(find(QString)), database, SLOT(find(QString)));
    QObject::connect(this, SIGNAL(save(QString, QImage, int)), database, SLOT(save(QString, QImage, int)));
    QObject::connect(database, SIGNAL(found(QString, QImage)), this, SLOT(foundSlot(QString, QImage)));
    QObject::connect(database, SIGNAL(notFound(QString)), this, SLOT(notFoundSlot(QString)));
    QObject::connect(&databaseThread, SIGNAL(finished()), database, SLOT(deleteLater()));
    databaseThread.start();
    emit connectDatabase();
}

void ThumbnailQueue::add(File* file) {
    emit find(file->getPath());
}

void ThumbnailQueue::foundSlot(QString path, QImage image) {
    emit done(path, image);
}

void ThumbnailQueue::notFoundSlot(QString path) {
    File* file = fileManager->getFileByPath(path);
    auto task = new ThumbnailWorker(this, file, config);
    queue.append(task);
    start();
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
}

void ThumbnailQueue::doneSlot(QString path, QImage image, int format) {
    emit save(path, image, format);
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
