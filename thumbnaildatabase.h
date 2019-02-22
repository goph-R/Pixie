#ifndef THUMBNAILDATABASE_H
#define THUMBNAILDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QImage>

class ThumbnailDatabase : public QObject
{
    Q_OBJECT

public:
    ThumbnailDatabase(QString path);
    virtual ~ThumbnailDatabase();

public slots:
    void connect();
    void find(QString path);
    void save(QString path, QImage image);

signals:
    void notFound(QString path);
    void found(QString path, QImage image);

private:
    QString databasePath;
    QSqlDatabase db;
};

#endif // THUMBNAILDATABASE_H
