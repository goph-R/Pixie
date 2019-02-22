#include "thumbnailqueue.h"
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
    auto database = new ThumbnailDatabase(config->getCacheFolder() + "thumbnails.s3db");
    database->moveToThread(&databaseThread);
    connect(this, &ThumbnailQueue::connectDatabase, database, &ThumbnailDatabase::connect);
    connect(this, &ThumbnailQueue::find, database, &ThumbnailDatabase::find);
    connect(this, &ThumbnailQueue::save, database, &ThumbnailDatabase::save);
    connect(database, &ThumbnailDatabase::found, this, &ThumbnailQueue::foundSlot);
    connect(database, &ThumbnailDatabase::notFound, this, &ThumbnailQueue::notFoundSlot);
    connect(&databaseThread, &QThread::finished, database, &ThumbnailDatabase::deleteLater);
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
    auto task = new ThumbnailRunner(file, config);
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
    auto worker = task->getWorker();
    connect(worker, &ThumbnailWorker::done, this, &ThumbnailQueue::doneSlot);
    connect(worker, &ThumbnailWorker::error, this, &ThumbnailQueue::errorSlot);
    connect(worker, &ThumbnailWorker::empty, this, &ThumbnailQueue::emptySlot);
}

void ThumbnailQueue::doneSlot(QString path, QImage image) {
    emit save(path, image);
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
