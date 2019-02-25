#ifndef IMAGERUNNER_H
#define IMAGERUNNER_H

#include <QObject>
#include <QRunnable>
#include <QSize>
#include <QMutex>

class ImageWorker;

class ImageRowWorker : public QRunnable
{
public:
    ImageRowWorker(ImageWorker* worker, QString path, QSize size, int row, int rowHeight);
    void run();

private:
    ImageWorker* worker;
    QString path;
    QSize size;
    int row;
    int rowHeight;
    QMutex workerMutex;
};

#endif // IMAGERUNNER_H
