#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTimer>

class Config;
class ThumbnailQueue;
class FolderTreeWidget;
class FileManager;
class FileListWidget;
class FileListItem;
class File;
class ViewWindow;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const char* startPath, QWidget *parent = nullptr);
    virtual ~MainWindow() override;
    virtual QSize sizeHint() const override;
    virtual void closeEvent(QCloseEvent* event) override;
    void setViewWindow(ViewWindow* value);
    FileManager* getFileManager();
    FileListWidget* getFileListWidget();
    void startWith(const char* path);
    void exitApplication();

public slots:
    void folderSelectionChanged();
    void fileSelectionChanged();
    void folderExpanded(QTreeWidgetItem*);
    void expandFoldersDone(QString path);
    void addFile(File*);
    void findFilesDone();
    void thumbnailDone(QString path, QImage image);
    void thumbnailError(QString path);
    void thumbnailEmpty(QString path);
    void backspacePressed();
    void enterPressed();
    void showSettings();
    void showAbout();
    void quitSlot();
    void pasteFiles();
    void execute(QListWidgetItem* item);

private:    
    void addDrives();
    void enterFolder(File* file);
    void showImage(QList<QListWidgetItem*> items);
    void setPathEditTo(QString path);
    void readSettings();
    void saveSettings();
    void pasteFile(QString srcPath);
    QString getDisplayPath(QString path);
    void setUiEnabled(bool value);
    void findFilesInCurrentFolder();
    void goToParentFolder();
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
    SettingsDialog* settingsDialog;
    QString filePathToExecute;
    const char* startPath;
};

#endif // MAINWINDOW_H
