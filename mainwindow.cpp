#include "mainwindow.h"
#include "filelistitem.h"

#include <QDebug>
#include <QPixmap>
#include "file.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{    
    config = new Config();
    fileManager = new FileManager();
    thumbnailQueue = new ThumbnailQueue(config);

    folderTreeWidget = new FolderTreeWidget();
    dockWidget = new QDockWidget("Folders");
    dockWidget->setWidget(folderTreeWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    fileListWidget = new FileListWidget(config);
    setCentralWidget(fileListWidget);
    currentFolder = nullptr;

    addDrives();

    connect(folderTreeWidget, &FolderTreeWidget::itemExpanded, this, &MainWindow::folderExpanded);
    connect(folderTreeWidget, &FolderTreeWidget::itemSelectionChanged, this, &MainWindow::folderSelectionChanged);
    connect(fileManager, &FileManager::folderAdded, folderTreeWidget, &FolderTreeWidget::addFolder);
    connect(fileManager, &FileManager::folderEmpty, folderTreeWidget, &FolderTreeWidget::removeLoadingText);
    connect(fileManager, &FileManager::fileAdded, this, &MainWindow::addFile);
    connect(fileManager, &FileManager::findFilesDone, this, &MainWindow::findFilesDone);
    connect(thumbnailQueue, &ThumbnailQueue::done, this, &MainWindow::thumbnailDone);
    connect(thumbnailQueue, &ThumbnailQueue::error, this, &MainWindow::thumbnailError);
}

MainWindow::~MainWindow() {    
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
    fileManager->findFiles(currentFolder);
}

void MainWindow::addFile(File* file) {
    if (file->getParent()->getPath() != currentFolder->getPath()) {
        return;
    }
    auto imageExtensions = config->getImageExtensions();
    if (file->isFolder() || imageExtensions.contains(file->getExtension())) {
        thumbnailQueue->add(file);
    }
    fileListWidget->createItem(file);
}

void MainWindow::findFilesDone() {
    thumbnailQueue->start();
}

void MainWindow::thumbnailDone(QString path, QImage image) {
    if (fileListWidget->isItemExist(path)) {
        auto item = fileListWidget->getItem(path);
        auto pixmap = QPixmap::fromImage(image);
        item->setPixmap(pixmap);
        fileListWidget->viewport()->update();
    }
}

void MainWindow::thumbnailError(QString path) {
    fileListWidget->setErrorPixmap(path);
}
