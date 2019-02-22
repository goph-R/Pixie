#include "foldertreewidget.h"

#include <QDir>
#include <QDebug>
#include <QHeaderView>
#include "foldertreeitem.h"

FolderTreeWidget::FolderTreeWidget() : QTreeWidget() {
    setHeaderHidden(true);
    folderIcon = QIcon(":/icons/folder.png");
    auto h = header();
    h->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    h->setStretchLastSection(false);
}

FolderTreeWidget::~FolderTreeWidget() {
    foreach (auto file, itemsByPath.values()) {
        delete file;
    }
    itemsByPath.clear();
}

FolderTreeItem* FolderTreeWidget::createItem(FolderTreeItem* parentItem, File* file, QIcon icon) {
    removeLoadingTextItem(parentItem);
    auto item = new FolderTreeItem(parentItem, file);
    item->setText(0, file->getName());
    item->setIcon(0, icon);
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    itemsByPath.insert(file->getPath(), item);
    addLoadingItem(item);
    return item;
}

void FolderTreeWidget::removeLoadingText(File* file) {
    auto item = getItem(file);
    removeLoadingTextItem(item);
}

void FolderTreeWidget::removeLoadingTextItem(FolderTreeItem* item) {
    if (item == nullptr) {
        return;
    }
    FolderTreeItem* child = static_cast<FolderTreeItem*>(item->child(0));
    if (child->isLoadingText()) {
        item->takeChild(0);
    }
}

void FolderTreeWidget::addFolder(File* file) {
    if (hasItem(file)) {
        return;
    }
    auto parentItem = getItem(file->getParent());
    createItem(parentItem, file, folderIcon);
}


void FolderTreeWidget::addLoadingItem(FolderTreeItem* item) {
    auto loadingItem = new FolderTreeItem(item, nullptr);
    loadingItem->loadingText = true;
    loadingItem->setText(0, "Loading..");
}

bool FolderTreeWidget::hasItem(File* file) {
    return itemsByPath.contains(file->getPath());
}

FolderTreeItem* FolderTreeWidget::getItem(File* file) {
    return itemsByPath.value(file->getPath());
}

