#include "filelistitem.h"

FileListItem::FileListItem(File* file) : QListWidgetItem() {
    this->file = file;
}

File* FileListItem::getFile() {
    return file;
}

void FileListItem::setPixmap(QPixmap value) {
    pixmap = value;
}
