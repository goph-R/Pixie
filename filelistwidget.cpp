#include "filelistwidget.h"

#include <QDebug>
#include "filelistdelegate.h"
#include "filelistitem.h"

FileListWidget::FileListWidget(Config* config) : QListWidget() {
    this->config = config;
    folderPixmap = QPixmap(":/icons/folder-big.png").scaled(112, 112, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    filePixmap = QPixmap(":/icons/file-big.png").scaled(112, 112, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imagePixmap = QPixmap(":/icons/image-big.png").scaled(112, 112, Qt::KeepAspectRatio, Qt::SmoothTransformation);
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

FileListItem* FileListWidget::createItem(File* file) {
    auto item = new FileListItem(file);
    auto imageExtensions = config->getImageExtensions();
    if (!file->isFolder()) {
        item->setPixmap(imageExtensions.contains(file->getExtension()) ? imagePixmap : filePixmap);
    }
    item->setText(file->getName());
    item->setData(Qt::UserRole, file->getPath());
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
