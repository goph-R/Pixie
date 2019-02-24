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
class File;
class ViewWindow;
class SettingsDialog;

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
    void exitApplication();

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
    void execute(QListWidgetItem* item);
    void showSettings();
    void showAbout();
    void quitSlot();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:    
    void enterFolder(File* file);
    void showImage(File* file);
    void setPathEditTo(QString path);
    void readSettings();
    void saveSettings();
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
    SettingsDialog* settingsDialog;
};

#endif // MAINWINDOW_H
