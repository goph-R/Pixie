#include "ui/foldertreeitem.h"

#include "domain/file.h"

#include <QDebug>

FolderTreeItem::FolderTreeItem(QTreeWidgetItem* parent, File* file) : QTreeWidgetItem (parent) {
    this->file = file;
    loadingText = false;
}

File* FolderTreeItem::getFile() {
    return file;
}

bool FolderTreeItem::isLoadingText() {
    return loadingText;
}

