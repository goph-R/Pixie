#include "domain/thumbnailqueue.h"

#include "config.h"
#include "file.h"
#include "filemanager.h"
#include "thumbnailworker.h"
#include "thumbnaildatabase.h"

#include <QDebug>

ThumbnailQueue::ThumbnailQueue(Config* config, FileManager* fileManager) : QObject() {
    threadPool = QThreadPool::globalInstance();
    active = 0;
    this->config = config;
    this->fileManager = fileManager;
    createDatabaseThread();
}

ThumbnailQueue::~ThumbnailQueue() {
    clear();
    databaseThread.quit();
    databaseThread.wait();
}

void ThumbnailQueue::createDatabaseThread() {
    auto database = new ThumbnailDatabase(config->getThumbnailsPath());
    database->moveToThread(&databaseThread);
    QObject::connect(this, SIGNAL(connectDatabaseSignal()), database, SLOT(connect()));
    QObject::connect(this, SIGNAL(findSignal(QString)), database, SLOT(find(QString)));
    QObject::connect(this, SIGNAL(saveSignal(QString, QImage, int)), database, SLOT(save(QString, QImage, int)));
    QObject::connect(this, SIGNAL(removeSignal(QString)), database, SLOT(remove(QString)));
    QObject::connect(database, SIGNAL(found(QString, QImage)), this, SLOT(foundSlot(QString, QImage)));
    QObject::connect(database, SIGNAL(notFound(QString)), this, SLOT(notFoundSlot(QString)));
    QObject::connect(&databaseThread, SIGNAL(finished()), database, SLOT(deleteLater()));
    databaseThread.start();
    emit connectDatabaseSignal();
}

void ThumbnailQueue::add(File* file, bool useCache) {
    if (useCache) {
        emit findSignal(file->getPath());
    } else {
        createTask(file);
    }
}

void ThumbnailQueue::foundSlot(QString path, QImage image) {
    emit doneSignal(path, image);
}

void ThumbnailQueue::notFoundSlot(QString path) {
    File* file = fileManager->getFileByPath(path);
    createTask(file);
}

void ThumbnailQueue::createTask(File* file) {
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
    int max = threadPool->maxThreadCount();
    while (queue.size() && active < max) {
        startNext();
    }
}

void ThumbnailQueue::startNext() {
    active++;
    auto task = queue.takeFirst();
    threadPool->start(task, QThread::NormalPriority);
}

void ThumbnailQueue::doneSlot(QString path, QImage image, int format) {
    emit saveSignal(path, image, format);
    emit doneSignal(path, image);
    workerFinished();
}

void ThumbnailQueue::errorSlot(QString path) {
    emit errorSignal(path);
    workerFinished();
}

void ThumbnailQueue::emptySlot(QString path) {
    emit removeSignal(path);
    emit emptySignal(path);
    workerFinished();
}

void ThumbnailQueue::workerFinished() {
    active--;
    start();
}
