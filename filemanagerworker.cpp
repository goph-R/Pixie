#include "filemanagerworker.h"

#include <QDir>
#include <QDebug>
#include <QTime>

FileManagerWorker::FileManagerWorker() : QObject() {
}

void FileManagerWorker::findFiles(QString folderPath) {
    auto entries = findEntries(folderPath, QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    foreach (auto entry, entries) {
        auto result = FoundFile();
        result.name = entry.fileName();
        result.folder = entry.isDir();
        result.extension = entry.suffix().toLower();
        result.folderPath = folderPath;
        emit foundFile(result);
    }
    emit findFilesDone();
}

void FileManagerWorker::findFolders(QString folderPath) {
    auto entries = findEntries(folderPath, QDir::NoDotAndDotDot | QDir::Dirs);
    if (!entries.size()) {
        emit folderEmpty(folderPath);
        return;
    }
    foreach (auto entry, entries) {
        auto result = FoundFolder();
        result.name = entry.fileName();
        result.folderPath = folderPath;
        emit foundFolder(result);
    }
}

QFileInfoList FileManagerWorker::findEntries(QString folderPath, QFlags<QDir::Filter> filter) {
    QDir dir(folderPath);
    dir.setFilter(filter);
    dir.setSorting(QDir::Name | QDir::DirsFirst);
    return dir.entryInfoList();
}
