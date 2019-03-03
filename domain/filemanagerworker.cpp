#include "domain/filemanagerworker.h"

#include <QDir>
#include <QTime>
#include <QImageReader>

#include <QDebug>

FileManagerWorker::FileManagerWorker() : QObject() {
}

void FileManagerWorker::findFiles(QString folderPath) {
    auto entries = findEntries(folderPath, QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    bool foundFolders = false;
    foreach (auto entry, entries) {
        auto result = FoundFile();
        result.name = entry.fileName() + (entry.isDir() ? "/" : "");
        result.folder = entry.isDir();
        result.extension = entry.suffix().toLower();
        result.folderPath = folderPath;
        emit foundFile(result);
        foundFolders |= result.folder;
    }
    emit findFilesDone(folderPath, foundFolders);
}

void FileManagerWorker::findFolders(QString folderPath) {
    auto entries = findEntries(folderPath, QDir::NoDotAndDotDot | QDir::Dirs);
    if (entries.size()) {
        foreach (auto entry, entries) {
            auto result = FoundFolder();
            result.name = entry.fileName() + "/";
            result.folderPath = folderPath;
            emit foundFolder(result);
        }
    } else {
        emit folderEmpty(folderPath);        
    }
}

void FileManagerWorker::expandFolders(QString path, QStringList folderPaths) {
    foreach (auto folderPath, folderPaths) {
        findFolders(folderPath);
    }
    emit expandFoldersDone(path);
}

QFileInfoList FileManagerWorker::findEntries(QString folderPath, QFlags<QDir::Filter> filter) {
    QDir dir(folderPath);
    dir.setFilter(filter);
    dir.setSorting(QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
    return dir.entryInfoList();
}
