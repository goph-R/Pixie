#ifndef FILEMANAGERWORKER_H
#define FILEMANAGERWORKER_H

#include <QDir>
#include <QFileInfoList>
#include <QObject>
#include <QImage>
#include <QStringList>
#include "domain/foundfile.h"
#include "domain/foundfolder.h"

class FileManagerWorker : public QObject
{
    Q_OBJECT

public:
    FileManagerWorker();

public slots:
    void findFiles(QString folderPath);
    void findFolders(QString folderPath);
    void expandFolders(QStringList folderPaths, QStringList allFolderPaths);

signals:
    void foundFile(FoundFile result);
    void foundFolder(FoundFolder result);
    void folderEmpty(QString path);
    void findFilesDone(QString path, bool foundFolders);
    void expandFoldersDone(QStringList allFoldersPath);

private:
    QFileInfoList findEntries(QString folderPath, QFlags<QDir::Filter> filters);
};

#endif // FILEMANAGERWORKER_H
