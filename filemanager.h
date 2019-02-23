#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QThread>
#include <QHash>
#include <QImage>
#include "config.h"
#include "file.h"
#include "foundfile.h"
#include "foundfolder.h"

class FileManager : public QObject
{
    Q_OBJECT

public:
    FileManager(Config* config);
    ~FileManager();
    File* getRoot();
    void findFolders(File* file);
    void expandFolders(QString path);
    void findFiles(File* file);
    File* getFileByPath(QString path);
    void loadImage(QString path);

public slots:
    void foundFile(FoundFile result);
    void foundFolder(FoundFolder result);
    void folderEmptySlot(QString folderPath);
    void findFilesDoneSlot();
    void expandFoldersDoneSlot(QStringList folderPaths);
    void imageLoadedSlot(const QImage image);

signals:
    void findFilesSignal(QString path);
    void findFoldersSignal(QString path);
    void folderAdded(File* file);
    void folderEmpty(File* file);
    void fileAdded(File* file);
    void findFilesDone();
    void expandFoldersSignal(QStringList folderPaths, QStringList allFolderPaths);
    void expandFoldersDone(QStringList allFolderPaths);
    void loadImageSignal(QString path);
    void imageLoaded(const QImage image);

private:
    QStringList imageExtensions;
    QThread workerThread;
    QHash<QString, File*> filesByPath;
    File* root;
    void findDrives();
    void createWorkerThread();
    void createRoot();
    File* createEntry(QString folderPath, QString name, bool folder);
};

#endif // FILEMANAGER_H
