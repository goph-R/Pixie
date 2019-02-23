#ifndef THUMBNAILTASKSIGNALS_H
#define THUMBNAILTASKSIGNALS_H

#include <QObject>
#include <QRunnable>
#include <QString>
#include <QImage>
#include "config.h"
#include "file.h"

class ThumbnailWorker : public QRunnable
{
public:
    ThumbnailWorker(QObject* receiver, File* file, Config* config);
    void run();

signals:
    void done(QString path, QImage thumbnail, int format);
    void error(QString path);
    void empty();

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

#endif // THUMBNAILTASKSIGNALS_H
