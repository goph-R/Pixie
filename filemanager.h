#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QThread>
#include <QHash>
#include "file.h"
#include "foundfile.h"
#include "foundfolder.h"

class FileManager : public QObject
{
    Q_OBJECT

public:
    FileManager();
    ~FileManager();
    File* getRoot();
    void findFolders(File* file);
    void findFiles(File* file);

public slots:
    void foundFile(FoundFile result);
    void foundFolder(FoundFolder result);
    void folderEmptySlot(QString folderPath);
    void findFilesDoneSlot();

signals:
    void findFilesSignal(QString path);
    void findFoldersSignal(QString path);
    void folderAdded(File* file);
    void folderEmpty(File* file);
    void fileAdded(File* file);
    void findFilesDone();

private:
    QThread workerThread;
    QHash<QString, File*> filesByPath;
    File* root;
    void findDrives();
    void createWorkerThread();
    void createRoot();
    File* createEntry(QString folderPath, QString name, bool folder);
};

#endif // FILEMANAGER_H
