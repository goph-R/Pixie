#ifndef FOLDERTREEWIDGET_H
#define FOLDERTREEWIDGET_H

#include <QTreeWidget>
#include <QHash>
#include "foldertreeitem.h"

class FolderTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    FolderTreeWidget();
    virtual ~FolderTreeWidget();
    FolderTreeItem* createItem(FolderTreeItem* parentItem, File* file, QIcon icon);
    bool hasItem(File* file);
    FolderTreeItem* getItem(File* file);
    void removeLoadingTextItem(FolderTreeItem* item);

public slots:
    void addFolder(File* file);
    void removeLoadingText(File* file);

private:
    QHash<QString, FolderTreeItem*> itemsByPath;
    void addLoadingItem(FolderTreeItem* item);
    QIcon folderIcon;
};

#endif // FOLDERTREEWIDGET_H
