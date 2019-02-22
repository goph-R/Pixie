#include "thumbnaildatabase.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QBuffer>
#include <QDebug>

ThumbnailDatabase::ThumbnailDatabase() : QObject() {
}

void ThumbnailDatabase::connect() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Users/gopher/.pixie/pixie.s3db");
}

ThumbnailDatabase::~ThumbnailDatabase() {
    if (db.open()) {
        db.close();
    }
}

void ThumbnailDatabase::find(QString path) {
    if (!db.open()) {
        emit notFound(path);
        return;
    }
    QSqlQuery query;
    QSqlRecord record;
    QImage image;
    query.prepare("SELECT image FROM thumbnail WHERE path = :path");
    query.bindValue(":path", path);
    query.exec();
    if (query.first()) {
        record = query.record();
        image.loadFromData(record.field("image").value().toByteArray(), "jpg");
        emit found(path, image);
    } else {
        emit notFound(path);
    }
}

void ThumbnailDatabase::save(QString path, QImage image) {
    if (!db.open()) {
        return;
    }
    QByteArray ba;
    QBuffer buffer(&ba);
    QSqlQuery query;
    buffer.open(QIODevice::WriteOnly | QIODevice::Truncate);
    image.save(&buffer, "jpg", 90);
    query.prepare("INSERT INTO thumbnail(path, image) VALUES (:path, :image)");
    query.bindValue(":path", path);
    query.bindValue(":image", buffer.data());
    query.exec();
    buffer.close();
}

