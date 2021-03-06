#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QThread>
#include <QHash>
#include <QImage>
#include <QStringList>
#include "domain/foundfile.h"
#include "domain/foundfolder.h"

class Config;
class File;

class FileManager : public QObject
{
    Q_OBJECT

public:
    FileManager(Config* config);
    ~FileManager();
    File* getRoot();
    void findFolders(File* file);
    void expandFolders(QString path);
    void findFiles(File* file, bool addDotDotFolder);
    File* getFileByPath(QString path);
    void loadImage(QString path);

public slots:
    void foundFile(FoundFile result);
    void foundFolder(FoundFolder result);
    void folderEmptySlot(QString folderPath);
    void findFilesDoneSlot(QString folderPath, bool foundFolders);
    void expandFoldersDoneSlot(QString path);

signals:
    void findFilesSignal(QString path);
    void findFoldersSignal(QString path);
    void folderAdded(File* file);
    void folderEmpty(File* file);
    void fileAdded(File* file);
    void findFilesDone();
    void expandFoldersSignal(QString path, QStringList folderPaths);
    void expandFoldersDone(QString path);

private:
    QStringList imageExtensions;
    QThread workerThread;
    QHash<QString, File*> filesByPath;
    File* root;
    void findDrives();
    void createWorkerThread();
    void createRoot();
    File* createEntry(QString folderPath, QString name, bool folder);
    File* dotDotFile;
};

#endif // FILEMANAGER_H
