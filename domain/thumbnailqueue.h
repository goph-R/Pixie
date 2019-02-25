#ifndef THUMBNAILQUEUE_H
#define THUMBNAILQUEUE_H

#include <QObject>
#include <QThreadPool>
#include <QImage>

class ThumbnailWorker;
class Config;
class File;
class FileManager;

class ThumbnailQueue : public QObject
{
    Q_OBJECT

public:
    ThumbnailQueue(Config* config, FileManager* fileManager);
    ~ThumbnailQueue();
    void add(File* file);
    void start();
    void clear();

public slots:
    void foundSlot(QString path, QImage image);
    void notFoundSlot(QString path);
    void doneSlot(QString path, QImage image, int format);
    void errorSlot(QString path);
    void emptySlot();

signals:    
    void connectDatabase();
    void find(QString path);
    void save(QString path, QImage image, int format);
    void done(QString path, QImage image);
    void error(QString path);

private:
    void createDatabaseThread();
    QList<ThumbnailWorker*> queue;
    QThreadPool* threadPool;
    Config* config;
    void startNext();
    int active;
    void workerFinished();
    QThread databaseThread;
    FileManager* fileManager;

};

#endif // THUMBNAILQUEUE_H
