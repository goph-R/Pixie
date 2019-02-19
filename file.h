#ifndef FILE_H
#define FILE_H

#include <QString>
#include <QList>

class File
{
public:
    File(File* parent, QString name, bool folder);
    ~File();
    QString getPath();
    QString getExtension();
    QString getName();
    File* getParent();
    void addChild(File* file);
    const QList<File*> getChildren();
    void removeChildren();
    bool isFolder();

private:
    File* parent;
    bool folder;
    QString name;
    QString path;
    QString extension;
    QList<File*> children;

    friend class FileManager;
};

#endif // FILE_H