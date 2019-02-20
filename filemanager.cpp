#include "filemanager.h"

#include <QDebug>
#include <QDir>
#include "filemanagerworker.h"

FileManager::FileManager() : QObject() {
    createWorkerThread();
    createRoot();
    findDrives();
}

FileManager::~FileManager() {
    workerThread.quit();
    workerThread.wait();
    delete root;
}

void FileManager::createWorkerThread() {
    auto worker = new FileManagerWorker();
    worker->moveToThread(&workerThread);
    connect(this, &FileManager::findFilesSignal, worker, &FileManagerWorker::findFiles);
    connect(this, &FileManager::findFoldersSignal, worker, &FileManagerWorker::findFolders);
    connect(worker, &FileManagerWorker::foundFolder, this, &FileManager::foundFolder);
    connect(worker, &FileManagerWorker::foundFile, this, &FileManager::foundFile);
    connect(worker, &FileManagerWorker::folderEmpty, this, &FileManager::folderEmptySlot);
    connect(worker, &FileManagerWorker::findFilesDone, this, &FileManager::findFilesDoneSlot);
    connect(&workerThread, &QThread::finished, worker, &FileManagerWorker::deleteLater);
    workerThread.start();
}

void FileManager::createRoot() {
    root = new File(nullptr, "", true);
}

File* FileManager::getRoot() {
    return root;
}

void FileManager::findDrives() {
    QDir dir;
    foreach (auto entry, dir.drives()) {
        File* file = new File(root, entry.filePath(), true);
        root->addChild(file);
        filesByPath.insert(file->getPath(), file);
    }
}

void FileManager::findFolders(File* file) {
    emit findFoldersSignal(file->getPath());
}

void FileManager::findFiles(File* file) {
    emit findFilesSignal(file->getPath());
}

void FileManager::folderEmptySlot(QString folderPath) {
    if (filesByPath.contains(folderPath)) {
        emit folderEmpty(filesByPath.value(folderPath));
    }
}

void FileManager::findFilesDoneSlot() {
    emit findFilesDone();
}

void FileManager::foundFolder(FoundFolder foundFolder) {
    QString folderPath = foundFolder.getFolderPath();
    QString path = folderPath + foundFolder.getName();
    if (filesByPath.contains(path)) {
        emit folderAdded(filesByPath.value(path));
    } else if (filesByPath.contains(folderPath)) {
        emit folderAdded(createEntry(folderPath, foundFolder.getName(), true));
    }
}

void FileManager::foundFile(FoundFile foundFile) {
    QString folderPath = foundFile.getFolderPath();
    QString path = folderPath + foundFile.getName();
    if (filesByPath.contains(path)) {
        emit fileAdded(filesByPath.value(path));
    } else if (filesByPath.contains(folderPath)) {
        auto file = createEntry(folderPath, foundFile.getName(), foundFile.isFolder());
        file->extension = foundFile.getExtension();
        emit fileAdded(file);
    }
}

File* FileManager::createEntry(QString folderPath, QString name, bool folder) {
    auto parent = filesByPath.value(folderPath);
    auto file = new File(parent, name, folder);
    filesByPath.insert(file->getPath(), file);
    return file;
}
