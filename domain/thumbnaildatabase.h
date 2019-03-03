#ifndef THUMBNAILDATABASE_H
#define THUMBNAILDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QImage>

class ThumbnailDatabase : public QObject
{
    Q_OBJECT

public:
    static const int FORMAT_PNG = 0;
    static const int FORMAT_JPEG = 1;

    ThumbnailDatabase(QString path);
    virtual ~ThumbnailDatabase();

public slots:
    void connect();
    void find(QString path);
    void save(QString path, QImage image, int format);
    void remove(QString path);

signals:
    void notFound(QString path);
    void found(QString path, QImage image);

private:
    QString databasePath;
    QSqlDatabase db;
    const char* getImageFormat(int format);
};

#endif // THUMBNAILDATABASE_H
