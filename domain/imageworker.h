#ifndef IMAGEWORKER_H
#define IMAGEWORKER_H

#include <QObject>
#include <QImage>
#include <QRect>
#include <QMutex>
#include <QFile>
#include <QImageReader>

class ImageWorker : public QObject
{
    Q_OBJECT

public:
    ImageWorker();
    void setPathToLoad(QString path); // mutex

public slots:
    void load(QString path);

signals:
    void loaded(const QString path, const QRect, const QImage image);
    void done(const QString path);

private:
    QMutex pathToLoadMutex;
    QString pathToLoad;
    QString getPathToLoad();
    void loadRow(QString path, QImageReader* reader, const QSize size, int row, int rowHeight);
};

#endif // IMAGEWORKER_H
