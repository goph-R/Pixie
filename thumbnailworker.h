#ifndef THUMBNAILTASKSIGNALS_H
#define THUMBNAILTASKSIGNALS_H

#include <QObject>
#include <QString>
#include <QImage>
#include "config.h"
#include "file.h"

class ThumbnailWorker : public QObject
{
    Q_OBJECT

public:
    ThumbnailWorker(File* file, Config* config);
    void run();

signals:
    void done(QString path, QImage thumbnail);
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
};

#endif // THUMBNAILTASKSIGNALS_H
