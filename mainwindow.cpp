#include <QPixmap>
#include <QDebug>
#include <QVBoxLayout>
#include <QLineEdit>

#include "mainwindow.h"
#include "viewwindow.h"
#include "filelistitem.h"
#include "file.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    currentFolder = nullptr;
    fileToSelect = nullptr;

    config = new Config();
    fileManager = new FileManager(config);
    thumbnailQueue = new ThumbnailQueue(config, fileManager);

    folderTreeWidget = new FolderTreeWidget();
    dockWidget = new QDockWidget("Folders");
    dockWidget->setWidget(folderTreeWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    fileListWidget = new FileListWidget(config);
    pathEdit = new QLineEdit();

    addDrives();

    auto w = new QWidget();
    auto v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(5);
    v->addWidget(pathEdit);
    v->addWidget(fileListWidget);

    setCentralWidget(w);

    connect(folderTreeWidget, &FolderTreeWidget::itemExpanded, this, &MainWindow::folderExpanded);
    connect(folderTreeWidget, &FolderTreeWidget::itemSelectionChanged, this, &MainWindow::folderSelectionChanged);
    connect(fileManager, &FileManager::folderAdded, folderTreeWidget, &FolderTreeWidget::addFolder);
    connect(fileManager, &FileManager::folderEmpty, folderTreeWidget, &FolderTreeWidget::removeLoadingText);
    connect(fileManager, &FileManager::fileAdded, this, &MainWindow::addFile);
    connect(fileManager, &FileManager::findFilesDone, this, &MainWindow::findFilesDone);
    connect(thumbnailQueue, &ThumbnailQueue::done, this, &MainWindow::thumbnailDone);
    connect(thumbnailQueue, &ThumbnailQueue::error, this, &MainWindow::thumbnailError);
    connect(fileListWidget, &FileListWidget::itemDoubleClicked, this, &MainWindow::execute);
    connect(fileListWidget, &FileListWidget::backspacePressed, this, &MainWindow::backspacePressed);
    connect(fileListWidget, &FileListWidget::enterPressed, this, &MainWindow::enterPressed);

}

MainWindow::~MainWindow() {    
}

FileManager* MainWindow::getFileManager() {
    return fileManager;
}

FileListWidget* MainWindow::getFileListWidget() {
    return fileListWidget;
}

void MainWindow::setViewWindow(ViewWindow* value) {
    viewWindow = value;
}

void MainWindow::closeEvent(QCloseEvent* event __attribute__((unused))) {
    delete thumbnailQueue;
    delete fileManager;
    delete config;
}

void MainWindow::addDrives() {
    auto icon = QIcon(":/icons/hard-drive.png");
    auto root = fileManager->getRoot();
    foreach (auto file, root->getChildren()) {
        auto item = folderTreeWidget->createItem(nullptr, file, icon);
        folderTreeWidget->addTopLevelItem(item);
    }
}

QSize MainWindow::sizeHint() const {
    return QSize(1280, 720);
}

void MainWindow::folderExpanded(QTreeWidgetItem* item) {
    auto folderItem = static_cast<FolderTreeItem*>(item);
    fileManager->findFolders(folderItem->getFile());
}

void MainWindow::folderSelectionChanged() {
    auto selectedItems = folderTreeWidget->selectedItems();
    if (selectedItems.size() != 1) {
        return;
    }
    auto folderItem = static_cast<FolderTreeItem*>(selectedItems.at(0));
    if (folderItem->isLoadingText()) {
        return;
    }
    thumbnailQueue->clear();
    fileListWidget->clear();
    currentFolder = folderItem->getFile();    
    setPathEditToCurrentFolder();
    fileManager->findFiles(currentFolder);
}

void MainWindow::setPathEditToCurrentFolder() {
    QString path = currentFolder->getPath();
    if (config->useBackslash()) {
        path = path.replace("/", "\\");
    }
    pathEdit->setText(path);
}

void MainWindow::addFile(File* file) {
    if (file->getParent()->getPath() != currentFolder->getPath()) {
        return;
    }
    fileListWidget->createItem(file);
    if (file->isFolder() || file->isImage()) {
        thumbnailQueue->add(file);
    }
}

void MainWindow::findFilesDone() {
    if (fileToSelect != nullptr) {
        fileListWidget->select(fileToSelect->getPath());
    }
    fileToSelect = nullptr;
}

void MainWindow::thumbnailDone(QString path, QImage image) {
    if (fileListWidget->hasItem(path)) {
        auto item = fileListWidget->getItem(path);
        auto pixmap = QPixmap::fromImage(image);
        item->setPixmap(pixmap);
        fileListWidget->viewport()->update();
    }
}

void MainWindow::thumbnailError(QString path) {
    fileListWidget->setErrorPixmap(path);
}

void MainWindow::enterFolder(File* file) {
    folderTreeWidget->clearSelection();
    if (!folderTreeWidget->hasItem(file)) {
        // TODO: check later with start up directory and with CLI
        fileManager->expandFolders(file->getPath());
        return;
    }
    folderTreeWidget->expandTo(file);
    folderTreeWidget->select(file);
}

void MainWindow::showImage(File* file) {
    if (isMaximized()) {
        viewWindow->setMaximized(true);
    } else {
        viewWindow->setMaximized(false);
        viewWindow->setGeometry(geometry());
    }
    viewWindow->setImage(file);
    hide();
}

void MainWindow::backspacePressed() {
    auto parent = currentFolder->getParent();
    if (parent->getPath() != "") {
        fileToSelect = currentFolder;
        folderTreeWidget->clearSelection();
        folderTreeWidget->select(parent);
    }
}

void MainWindow::enterPressed() {
    auto items = fileListWidget->selectedItems();
    if (items.size() != 1) {
        return;
    }
    execute(items.at(0));
}

void MainWindow::execute(QListWidgetItem* item) {
    auto file = static_cast<FileListItem*>(item)->getFile();
    if (file->isFolder()) {
        enterFolder(file);
    } else if (file->isImage()) {
        showImage(file);
    }
}
