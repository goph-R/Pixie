#include "ui/foldertreewidget.h"

#include <QDir>
#include <QHeaderView>
#include <QSizePolicy>
#include "domain/file.h"
#include "ui/foldertreeitem.h"

#include <QDebug>

FolderTreeWidget::FolderTreeWidget() : QTreeWidget() {
    setHeaderHidden(true);
    folderIcon = QIcon(":/icons/folder.png");
    auto h = header();
#if QT_VERSION >= 0x050000
    h->setSectionResizeMode(0, QHeaderView::ResizeToContents);
#else
    h->setResizeMode(0, QHeaderView::ResizeToContents);
#endif
    h->setStretchLastSection(false);
}

FolderTreeWidget::~FolderTreeWidget() {
    itemsByPath.clear();
}

FolderTreeItem* FolderTreeWidget::createItem(FolderTreeItem* parentItem, File* file, QIcon icon) {
    removeLoadingTextItem(parentItem);
    auto item = new FolderTreeItem(parentItem, file);
    item->setText(0, file->getName());
    item->setIcon(0, icon);
    item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
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
    if (child != nullptr && child->isLoadingText()) {
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

void FolderTreeWidget::select(File* file) {
    if (!hasItem(file)) {
        return;
    }
    auto item = getItem(file);
    item->setSelected(true);
    QModelIndexList indices = selectionModel()->selectedIndexes();
    scrollTo(indices.at(0));
    setCurrentIndex(indices.at(0));
}

void FolderTreeWidget::expandTo(File* file) {
    blockSignals(true);
    auto current = file->getParent();
    while (current->getPath() != "") {
        auto item = getItem(current);
        item->setExpanded(true);
        current = current->getParent();
    }
    blockSignals(false);
}
