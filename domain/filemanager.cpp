#include "domain/filemanager.h"

#include <QDebug>
#include "domain/config.h"
#include "domain/file.h"
#include "domain/filemanagerworker.h"

#include <QDir>

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
    QObject::connect(this, SIGNAL(findFilesSignal(QString)), worker, SLOT(findFiles(QString)));
    QObject::connect(this, SIGNAL(findFoldersSignal(QString)), worker, SLOT(findFolders(QString)));
    QObject::connect(this, SIGNAL(expandFoldersSignal(QString, QStringList)), worker, SLOT(expandFolders(QString, QStringList)));
    QObject::connect(worker, SIGNAL(foundFolder(FoundFolder)), this, SLOT(foundFolder(FoundFolder)));
    QObject::connect(worker, SIGNAL(foundFile(FoundFile)), this, SLOT(foundFile(FoundFile)));
    QObject::connect(worker, SIGNAL(folderEmpty(QString)), this, SLOT(folderEmptySlot(QString)));
    QObject::connect(worker, SIGNAL(findFilesDone(QString, bool)), this, SLOT(findFilesDoneSlot(QString, bool)));
    QObject::connect(worker, SIGNAL(expandFoldersDone(QString)), this, SLOT(expandFoldersDoneSlot(QString)));
    QObject::connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
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
    // gather folders
    auto expandPath = path;
    if (expandPath.endsWith("/")) {
        expandPath = expandPath.left(expandPath.length() - 1);
    }
    auto parts = expandPath.split("/");
    QString current = "";
    QStringList folderPaths;
    foreach (auto part, parts) {
        current += part + "/";
        QFileInfo info(current);
        if (!info.exists()) {
            return;
        }
        folderPaths << current;
    }

    // expand folders one by one
    if (!folderPaths.size()) {
        emit expandFoldersDone(path);
    } else {
        emit expandFoldersSignal(path, folderPaths);
    }

}

void FileManager::expandFoldersDoneSlot(QString path) {
    emit expandFoldersDone(path);
}

void FileManager::findFiles(File* file) {
    emit findFilesSignal(file->getPath());
}

void FileManager::folderEmptySlot(QString folderPath) {
    if (filesByPath.contains(folderPath)) {
        emit folderEmpty(filesByPath.value(folderPath));
    }
}

void FileManager::findFilesDoneSlot(QString folderPath, bool foundFolders) {
    if (!foundFolders && filesByPath.contains(folderPath)) {
        qDebug() << folderPath;
        emit folderEmpty(filesByPath.value(folderPath));
    }
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
    return filesByPath.contains(path) ? filesByPath.value(path) : nullptr;
}

