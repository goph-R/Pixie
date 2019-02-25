#ifndef IMAGEWORKER_H
#define IMAGEWORKER_H

#include <QObject>
#include <QImage>
#include <QRect>
#include <QMutex>
#include <QFile>
#include <QImageReader>
#include <QThreadPool>

class ImageWorker : public QObject
{
    Q_OBJECT

public:
    ImageWorker();
    ~ImageWorker();
    void setPathToLoad(QString path);
    const QString getPathToLoad();

public slots:
    void load(QString path);
    void loadedSlot(const QString path, const QRect, const QImage image);
    void doneSlot(const QString path);

signals:
    void loaded(const QString path, const QRect, const QImage image);
    void done(const QString path);

private:
    QMutex pathToLoadMutex;
    QString pathToLoad;
    void loadRow(QString path, QImageReader* reader, const QSize size, int row, int rowHeight);
    QThreadPool* threadPool;
};

#endif // IMAGEWORKER_H
