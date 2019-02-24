#include "ui/filelistitem.h"

#include "domain/file.h"

FileListItem::FileListItem(File* file) : QListWidgetItem() {
    this->file = file;
}

File* FileListItem::getFile() {
    return file;
}

void FileListItem::setPixmap(QPixmap value) {
    pixmap = value;
}
