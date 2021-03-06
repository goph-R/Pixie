#ifndef FOLDERTREEWIDGET_H
#define FOLDERTREEWIDGET_H

#include <QTreeWidget>
#include <QHash>

class Pixie;
class File;
class FolderTreeItem;

class FolderTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    FolderTreeWidget(Pixie* pixie);
    virtual ~FolderTreeWidget() override;
    FolderTreeItem* createItem(FolderTreeItem* parentItem, File* file, QIcon icon);
    bool hasItem(File* file);
    FolderTreeItem* getItem(File* file);
    void removeLoadingTextItem(FolderTreeItem* item);
    void select(File* file);
    void expandTo(File* file);

public slots:
    void addFolder(File* file);
    void removeLoadingText(File* file);

private:
    QHash<QString, FolderTreeItem*> itemsByPath;
    void addLoadingItem(FolderTreeItem* item);
    QIcon folderIcon;
};

#endif // FOLDERTREEWIDGET_H
