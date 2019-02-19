#ifndef FILEMANAGERWORKER_H
#define FILEMANAGERWORKER_H

#include <QDir>
#include <QFileInfoList>
#include <QObject>
#include "foundfile.h"
#include "foundfolder.h"

class FileManagerWorker : public QObject
{
    Q_OBJECT

public:
    FileManagerWorker();

public slots:
    void findFiles(QString folderPath);
    void findFolders(QString folderPath);

signals:
    void foundFile(FoundFile result);
    void foundFolder(FoundFolder result);
    void folderEmpty(QString path);
    void findFilesDone();

private:
    QFileInfoList findEntries(QString folderPath, QFlags<QDir::Filter> filters);
};

#endif // FILEMANAGERWORKER_H
