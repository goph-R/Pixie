#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QListWidget>
#include <QHash>
#include "file.h"
#include "config.h"

class FileListDelegate;
class FileListItem;

class FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    FileListWidget(Config* config);
    ~FileListWidget();
    void resizeImages();
    FileListItem* createItem(File* file);
    bool hasItem(QString path);
    FileListItem* getItem(QString path);
    void setErrorPixmap(QString path);
    virtual void clear();

private:
    Config* config;
    QHash<QString, FileListItem*> itemsByPath;
    FileListDelegate* delegate;
    QPixmap folderPixmap;
    QPixmap filePixmap;
    QPixmap imagePixmap;
    QPixmap imageErrorPixmap;

    friend class FileListDelegate;
};

#endif // FILELISTWIDGET_H
