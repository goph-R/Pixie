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
    QString getDisplayName();
    File* getParent();
    void addChild(File* file);
    QList<File*> getChildren();
    void removeChildren();
    bool isFolder();
    bool isImage();
    int getWidth();
    int getHeight();

private:
    File* parent;
    bool folder;
    bool image;
    QString name;
    QString path;
    QString extension;
    int width;
    int height;
    QList<File*> children;

    friend class FileManager;
};

#endif // FILE_H
