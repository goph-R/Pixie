#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDockWidget>
#include "foldertreewidget.h"
#include "filelistwidget.h"
#include "filemanager.h"
#include "config.h"
#include "thumbnailqueue.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Config* config, FileManager* fileManager, ThumbnailQueue* thumbnailQueue, QWidget *parent = nullptr);
    virtual ~MainWindow() override;
    virtual QSize sizeHint() const override;
    void addDrives();

public slots:
    void folderSelectionChanged();    
    void folderExpanded(QTreeWidgetItem*);
    void addFile(File*);
    void findFilesDone();
    void thumbnailDone(QString path, QImage image);
    void thumbnailError(QString path);

private:    
    Config* config;
    FileManager* fileManager;
    ThumbnailQueue* thumbnailQueue;
    QDockWidget* dockWidget;
    FolderTreeWidget* folderTreeWidget;
    FileListWidget* fileListWidget;
    File* currentFolder;
};

#endif // MAINWINDOW_H
