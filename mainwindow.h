#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include "foldertreewidget.h"
#include "filelistwidget.h"
#include "filemanager.h"
#include "config.h"
#include "thumbnailqueue.h"

class ViewWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;
    virtual QSize sizeHint() const override;
    void addDrives();
    virtual void closeEvent(QCloseEvent* event) override;
    void setViewWindow(ViewWindow* value);

public slots:
    void folderSelectionChanged();    
    void folderExpanded(QTreeWidgetItem*);
    void addFile(File*);
    void findFilesDone();
    void thumbnailDone(QString path, QImage image);
    void thumbnailError(QString path);
    void itemDoubleClicked(QListWidgetItem* item);

private:    
    void selectFolder(File* file);
    void showImage(File* file);
    Config* config;
    FileManager* fileManager;
    ThumbnailQueue* thumbnailQueue;
    QDockWidget* dockWidget;
    FolderTreeWidget* folderTreeWidget;
    FileListWidget* fileListWidget;
    File* currentFolder;
    ViewWindow* viewWindow;
};

#endif // MAINWINDOW_H
