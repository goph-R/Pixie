#ifndef FOLDERTREEWIDGET_H
#define FOLDERTREEWIDGET_H

#include <QTreeWidget>
#include <QHash>

class File;
class FolderTreeItem;

class FolderTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    FolderTreeWidget();
    virtual ~FolderTreeWidget() override;
    FolderTreeItem* createItem(FolderTreeItem* parentItem, File* file, QIcon icon);
    bool hasItem(File* file);
    FolderTreeItem* getItem(File* file);
    void removeLoadingTextItem(FolderTreeItem* item);
    void select(File* file);
    void expandTo(File* file);
    void setPreferredWidth(int width);
    virtual QSize sizeHint() const override;

protected:
    virtual void resizeEvent(QResizeEvent* event) override;

public slots:
    void addFolder(File* file);
    void removeLoadingText(File* file);

private:
    QHash<QString, FolderTreeItem*> itemsByPath;
    void addLoadingItem(FolderTreeItem* item);
    QIcon folderIcon;
    int preferredWidth;
};

#endif // FOLDERTREEWIDGET_H
