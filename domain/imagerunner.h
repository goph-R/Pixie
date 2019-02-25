#ifndef IMAGERUNNER_H
#define IMAGERUNNER_H

#include <QObject>
#include <QRunnable>
#include <QSize>
#include <QString>

class ImageRunner : public QRunnable
{
public:
    ImageRunner(QObject* receiver, QString path, QSize size, int row, int rowHeight);
    void run();

private:
    QObject* receiver;
    QString path;
    QSize size;
    int row;
    int rowHeight;
};

#endif // IMAGERUNNER_H
