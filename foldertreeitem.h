#ifndef FOLDERTREEWIDGETITEM_H
#define FOLDERTREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include "file.h"

class FolderTreeItem : public QTreeWidgetItem
{

public:
    FolderTreeItem(QTreeWidgetItem* parent, File* file);
    File* getFile();
    bool isLoadingText();

private:
    File* file;
    bool loadingText;

    friend class FolderTreeWidget;

};

#endif // FOLDERTREEWIDGETITEM_H
