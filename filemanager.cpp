#include "filemanager.h"

#include <QDebug>
#include <QDir>
#include "filemanagerworker.h"

FileManager::FileManager(Config* config) : QObject() {
    imageExtensions = config->getImageExtensions();
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
    connect(this, &FileManager::loadImageSignal, worker, &FileManagerWorker::loadImage);
    connect(worker, &FileManagerWorker::foundFolder, this, &FileManager::foundFolder);
    connect(worker, &FileManagerWorker::foundFile, this, &FileManager::foundFile);
    connect(worker, &FileManagerWorker::folderEmpty, this, &FileManager::folderEmptySlot);
    connect(worker, &FileManagerWorker::findFilesDone, this, &FileManager::findFilesDoneSlot);
    connect(worker, &FileManagerWorker::expandFoldersDone, this, &FileManager::expandFoldersDoneSlot);
    connect(worker, &FileManagerWorker::imageLoaded, this, &FileManager::imageLoadedSlot);
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

void FileManager::expandFolders(QString path) {
    auto expandPath = path;
    if (expandPath.endsWith("/")) {
        expandPath = expandPath.left(expandPath.length() - 1);
    }
    auto parts = expandPath.split("/");
    auto current = parts.at(0) + "/";
    parts.removeFirst();
    QStringList folderPaths;
    QStringList allFolderPaths;
    foreach (auto part, parts) {
        current += part + "/";
        allFolderPaths << current;
        if (!filesByPath.contains(current)) {
            folderPaths << current;
        }
    }
    if (!folderPaths.size()) {
        emit expandFoldersDone(allFolderPaths);
    } else {
        emit expandFoldersSignal(folderPaths, allFolderPaths);
    }

}

void FileManager::expandFoldersDoneSlot(QStringList allFolderPaths) {
    emit expandFoldersDone(allFolderPaths);
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
    File* file = nullptr;
    if (filesByPath.contains(path)) {
        file = filesByPath.value(path);
    } else if (filesByPath.contains(folderPath)) {
        file = createEntry(folderPath, foundFile.getName(), foundFile.isFolder());
        file->extension = foundFile.getExtension();
        file->image = imageExtensions.contains(file->extension);
    }
    if (file != nullptr) {
        emit fileAdded(file);
        if (foundFile.isFolder()) {
            emit folderAdded(file);
        }
    }
}

File* FileManager::createEntry(QString folderPath, QString name, bool folder) {
    auto parent = filesByPath.value(folderPath);
    auto file = new File(parent, name, folder);
    parent->addChild(file);
    filesByPath.insert(file->getPath(), file);
    return file;
}

File* FileManager::getFileByPath(QString path) {
    return filesByPath.value(path);
}

void FileManager::loadImage(QString path) {
    emit loadImageSignal(path);
}

void FileManager::imageLoadedSlot(const QImage image) {
    emit imageLoaded(image);
}

