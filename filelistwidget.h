#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QListWidget>
#include <QHash>
#include "filelistitem.h"
#include "file.h"

class FileListDelegate;

class FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    FileListWidget();
    ~FileListWidget();
    FileListItem* createItem(File* file, QPixmap pixmap);
    bool isItemExist(QString path);
    FileListItem* getItem(QString path);
    virtual void clear();

private:
    QHash<QString, FileListItem*> itemsByPath;
    FileListDelegate* delegate;
};

#endif // FILELISTWIDGET_H
