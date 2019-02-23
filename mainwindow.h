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
    FileManager* getFileManager();
    FileListWidget* getFileListWidget();

public slots:
    void folderSelectionChanged();
    void fileSelectionChanged();
    void folderExpanded(QTreeWidgetItem*);    
    void addFile(File*);
    void findFilesDone();
    void thumbnailDone(QString path, QImage image);
    void thumbnailError(QString path);
    void backspacePressed();
    void enterPressed();

private:    
    void enterFolder(File* file);
    void showImage(File* file);
    void execute(QListWidgetItem* item);
    void setPathEditTo(QString path);
    QString getDisplayPath(QString path);
    Config* config;
    FileManager* fileManager;
    ThumbnailQueue* thumbnailQueue;
    QDockWidget* dockWidget;
    FolderTreeWidget* folderTreeWidget;
    FileListWidget* fileListWidget;
    QLineEdit* pathEdit;
    File* currentFolder;
    File* fileToSelect;
    ViewWindow* viewWindow;
};

#endif // MAINWINDOW_H
