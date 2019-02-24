#include "ui/filelistwidget.h"

#include <QShortcut>
#include <QKeyEvent>
#include "domain/file.h"
#include "domain/config.h"
#include "ui/filelistdelegate.h"
#include "ui/filelistitem.h"

#include <QDebug>

FileListWidget::FileListWidget(Config* config) : QListWidget() {
    this->config = config;
    createPixmaps();

    delegate = new FileListDelegate(this);
    setItemDelegate(delegate);
    setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setUniformItemSizes(true);

    new QShortcut(QKeySequence(Qt::Key_Backspace), this, SLOT(backspacePressedSlot()));
}

void FileListWidget::createPixmaps() {
    folderPixmap = createPixmap(":/icons/folder-big.png");
    filePixmap = createPixmap(":/icons/file-big.png");
    imagePixmap = createPixmap(":/icons/image-big.png");
    imageErrorPixmap = createPixmap(":/icons/image-error-big.png");
}

QPixmap FileListWidget::createPixmap(QString path) {
    int size = config->getThumbnailSize();
    return QPixmap(path).scaled(size - 8, size - 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

FileListWidget::~FileListWidget() {
    delete delegate;
}

FileListItem* FileListWidget::createItem(File* file) {
    auto item = new FileListItem(file);
    if (!file->isFolder()) {
        item->setPixmap(file->isImage() ? imagePixmap : filePixmap);
    }
    int thumbnailSize = config->getThumbnailSize();
    item->setText(file->getName());
    item->setData(Qt::UserRole, file->getPath());
    item->setSizeHint(QSize(thumbnailSize + 4, thumbnailSize + 37));
    addItem(item);
    itemsByPath.insert(file->getPath(), item);
    return item;
}

bool FileListWidget::hasItem(QString path) {
    return itemsByPath.contains(path);
}

FileListItem* FileListWidget::getItem(QString path) {
    return itemsByPath.value(path);
}

void FileListWidget::setErrorPixmap(QString path) {
    if (hasItem(path)) {
        auto item = getItem(path);
        item->setPixmap(imageErrorPixmap);
        viewport()->update();
    }
}

void FileListWidget::clear() {
    QListWidget::clear();
    itemsByPath.clear();
}

void FileListWidget::select(QString path) {
    if (!hasItem(path)) {
        return;
    }
    auto item = getItem(path);
    item->setSelected(true);
    QModelIndexList indices = selectionModel()->selectedIndexes();
    scrollTo(indices.at(0));
    setCurrentIndex(indices.at(0));
}


void FileListWidget::backspacePressedSlot() {
    emit backspacePressed();
}

void FileListWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return) {
        emit enterPressed();
    } else {
        QListWidget::keyPressEvent(event);
    }
}
