#include "filelistwidget.h"

#include <QDebug>
#include "filelistitem.h"
#include "filelistdelegate.h"

FileListWidget::FileListWidget() : QListWidget() {
    delegate = new FileListDelegate(this);
    setItemDelegate(delegate);
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setUniformItemSizes(true);
}

FileListWidget::~FileListWidget() {
    delete delegate;
}

FileListItem* FileListWidget::createItem(File* file, QPixmap pixmap) {
    auto item = new FileListItem(file);
    item->setText(file->getName());
    item->setData(Qt::UserRole, file->getPath());
    item->setPixmap(pixmap);
    item->setSizeHint(QSize(132, 165));
    addItem(item);
    itemsByPath.insert(file->getPath(), item);
    return item;
}

bool FileListWidget::isItemExist(QString path) {
    return itemsByPath.contains(path);
}

FileListItem* FileListWidget::getItem(QString path) {
    return itemsByPath.value(path);
}

void FileListWidget::clear() {
    QListWidget::clear();
    itemsByPath.clear();
}
