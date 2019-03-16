#include "ui/foldertreewidget.h"

#include <QDir>
#include <QHeaderView>
#include <QSizePolicy>
#include "pixie.h"
#include "theme.h"
#include "domain/file.h"
#include "ui/foldertreeitem.h"

#include <QDebug>

FolderTreeWidget::FolderTreeWidget(Pixie* pixie) : QTreeWidget() {
    auto theme = pixie->getTheme();
    folderIcon = QIcon(theme->getPixmap(":/icons/folder.png"));
    setHeaderHidden(true);
    setAlternatingRowColors(true);
    header()->setStretchLastSection(true);
}

FolderTreeWidget::~FolderTreeWidget() {
    itemsByPath.clear();
}

FolderTreeItem* FolderTreeWidget::createItem(FolderTreeItem* parentItem, File* file, QIcon icon) {
    removeLoadingTextItem(parentItem);
    auto item = new FolderTreeItem(parentItem, file);
    item->setText(0, file->getDisplayName());
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
