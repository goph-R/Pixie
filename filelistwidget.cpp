#include "filelistwidget.h"

#include <QDebug>
#include "filelistdelegate.h"
#include "filelistitem.h"

FileListWidget::FileListWidget(Config* config) : QListWidget() {
    this->config = config;
    resizeImages();

    delegate = new FileListDelegate(this);
    setItemDelegate(delegate);
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setUniformItemSizes(true);
}

void FileListWidget::resizeImages() {
    int size = config->getThumbnailSize();
    folderPixmap = QPixmap(":/icons/folder-big.png").scaled(size - 8, size - 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    filePixmap = QPixmap(":/icons/file-big.png").scaled(size - 8, size - 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imagePixmap = QPixmap(":/icons/image-big.png").scaled(size - 8, size - 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageErrorPixmap = QPixmap(":/icons/image-error-big.png").scaled(size - 8, size - 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
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
    int thumbnailSize = config->getThumbnailSize();
    item->setText(file->getName());
    item->setData(Qt::UserRole, file->getPath());
    item->setSizeHint(QSize(thumbnailSize + 4, thumbnailSize + 37));
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

void FileListWidget::setErrorPixmap(QString path) {
    if (isItemExist(path)) {
        auto item = getItem(path);
        item->setPixmap(imageErrorPixmap);
        viewport()->update();
    }
}

void FileListWidget::clear() {
    QListWidget::clear();
    itemsByPath.clear();
}
