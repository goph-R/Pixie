#include "domain/thumbnaildatabase.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QBuffer>
#include <QSqlError>
#include <QSqlDriver>

#include <QDebug>

ThumbnailDatabase::ThumbnailDatabase(QString path) : QObject() {
    databasePath = path;
}

void ThumbnailDatabase::connect() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databasePath);
}

ThumbnailDatabase::~ThumbnailDatabase() {
    if (db.open()) {
        db.close();
    }
}

const char* ThumbnailDatabase::getImageFormat(int format) {
    return format == FORMAT_PNG ? "png" : "jpg";
}

void ThumbnailDatabase::find(QString path) {
    if (!db.open()) {
        emit notFound(path);
        return;
    }
    QSqlQuery query;
    QSqlRecord record;
    QImage image;
    query.prepare("SELECT format, image FROM thumbnail WHERE path = :path");
    query.bindValue(":path", path);
    if (!query.exec()) {
    }
    if (query.first()) {
        record = query.record();
        auto format = record.field("format").value().toInt();
        auto data = record.field("image").value().toByteArray();
        image.loadFromData(data, getImageFormat(format));
        emit found(path, image);
    } else {
        emit notFound(path);
    }
}

void ThumbnailDatabase::save(QString path, QImage image, int format) {
    if (!db.open()) {
        return;
    }
    QByteArray ba;
    QBuffer buffer(&ba);
    QSqlQuery query;
    buffer.open(QIODevice::WriteOnly | QIODevice::Truncate);
    image.save(&buffer, getImageFormat(format), 85); // TODO: settings
    query.prepare("INSERT INTO thumbnail (path, format, image) VALUES (:path, :format, :image)");
    query.bindValue(":path", path);
    query.bindValue(":format", format);
    query.bindValue(":image", buffer.data());
    if (!query.exec()) {
    }
    buffer.close();
}

void ThumbnailDatabase::remove(QString path) {
    if (!db.open()) {
        return;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM thumbnail WHERE path = :path");
    query.bindValue(":path", path);
    if (!query.exec()) {
    }
}

