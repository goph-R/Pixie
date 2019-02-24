#ifndef THUMBNAILWORKER_H
#define THUMBNAILWORKER_H

#include <QRunnable>
#include <QObject>
#include <QImage>
#include <QStringList>

class File;
class Config;

class ThumbnailWorker : public QRunnable
{
public:
    ThumbnailWorker(QObject* receiver, File* file, Config* config);
    void run();

private:
    bool folder;
    QString getFirstImagePath();
    QString path;
    QStringList imageNameFilters;
    bool createThumbnail(QString path);
    int size;
    QImage image;
    QObject* receiver;
};

#endif // THUMBNAILWORKER_H
