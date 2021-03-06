#ifndef FILELISTWIDGETITEM_H
#define FILELISTWIDGETITEM_H

#include <QListWidgetItem>
#include <QPixmap>

class File;

class FileListItem : public QListWidgetItem
{
public:
    FileListItem(File* file);
    File* getFile();
    void setPixmap(QPixmap pixmap);
    void setErrorPixmap();

private:
    File* file;
    QPixmap pixmap;

    friend class FileListDelegate;
};

#endif // FILELISTWIDGETITEM_H
