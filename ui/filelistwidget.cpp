#include "ui/filelistwidget.h"

#include <QShortcut>
#include <QKeyEvent>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QScrollBar>
#include <QDebug>
#include "domain/file.h"
#include "domain/config.h"
#include "ui/filelistdelegate.h"
#include "ui/filelistitem.h"

FileListWidget::FileListWidget(Config* config) : QListWidget() {
    this->config = config;
    createPixmaps();

    delegate = new FileListDelegate(this);
    setItemDelegate(delegate);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    verticalScrollBar()->setSingleStep(((config->getThumbnailSize() + 37) / 3)); // TODO
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Static);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformItemSizes(true);

    new QShortcut(QKeySequence(Qt::Key_Backspace), this, SLOT(backspacePressedSlot()));
    new QShortcut(QKeySequence("Ctrl+C"), this, SLOT(copySelection()));
}

void FileListWidget::createPixmaps() {
    folderPixmap = createPixmap(":/icons/folder-big.png");
    filePixmap = createPixmap(":/icons/file-big.png");
    imagePixmap = createPixmap(":/icons/image-big.png");
    imageErrorPixmap = createPixmap(":/icons/image-error-big.png");
    backPixmap = createPixmap(":/icons/back-big.png");
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
    item->setText(file->getDisplayName());
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

void FileListWidget::clearPixmap(QString path) {
    if (hasItem(path)) {
        auto item = getItem(path);
        item->setPixmap(nullPixmap);
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
    clearSelection();
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

QList<File*> FileListWidget::getFiles() {
    QList<File*> result;
    for(int i = 0; i < count(); ++i) {
        auto fileListItem = static_cast<FileListItem*>(item(i));
        result.append(fileListItem->getFile());
    }
    return result;
}

int FileListWidget::countImages() {
    int result = 0;
    foreach (auto file, getFiles()) {
        if (file->isImage()) {
            ++result;
        }
    }
    return result;
}

int FileListWidget::countFolders() {
    int result = 0;
    foreach (auto file, getFiles()) {
        if (file->isFolder() && !file->isDotDot()) {
            ++result;
        }
    }
    return result;
}

void FileListWidget::copySelection() {
    auto items = selectedItems();
    if (items.count() < 1) {
        return;
    }
    auto clipboard = QApplication::clipboard();
    auto mimeData = new QMimeData();
    QList<QUrl> urls;
    foreach (auto item, items) {
        auto fileItem = static_cast<FileListItem*>(item);
        auto file = fileItem->getFile();
        if (!file->isDotDot()) {
            urls.append(QUrl::fromLocalFile(file->getPath()));
        }
    }
    mimeData->setUrls(urls);
    clipboard->setMimeData(mimeData);
}

