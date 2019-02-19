#ifndef THUMBNAILTASKSIGNALS_H
#define THUMBNAILTASKSIGNALS_H

#include <QObject>
#include <QString>
#include <QImage>

class ThumbnailWorker : public QObject
{
    Q_OBJECT

public:
    ThumbnailWorker(QString path);
    void run();

signals:
    void done(QString path, QImage thumbnail);
    void error(QString path);

private:
    QString path;
};

#endif // THUMBNAILTASKSIGNALS_H
