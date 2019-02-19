#ifndef FILELISTWIDGETITEM_H
#define FILELISTWIDGETITEM_H

#include <QListWidgetItem>
#include <QPixmap>
#include "file.h"

class FileListItem : public QListWidgetItem
{
public:
    FileListItem(File* file);
    File* getFile();
    void setPixmap(QPixmap pixmap);

private:
    File* file;
    QPixmap pixmap;

    friend class FileListDelegate;
};

#endif // FILELISTWIDGETITEM_H
