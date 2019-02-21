#include <QPixmap>
#include <QDebug>

#include "mainwindow.h"
#include "viewwindow.h"
#include "filelistitem.h"
#include "file.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    config = new Config();
    fileManager = new FileManager(config);
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
    connect(fileListWidget, &FileListWidget::itemDoubleClicked, this, &MainWindow::itemDoubleClicked);
}

MainWindow::~MainWindow() {    
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
    folderItem->setExpanded(true);
    fileManager->findFiles(currentFolder);
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

void MainWindow::itemDoubleClicked(QListWidgetItem* i) {
    auto item = static_cast<FileListItem*>(i);
    auto file = item->getFile();
    if (file->isFolder()) {
        selectFolder(file);
    } else if (file->isImage()) {
        showImage(file);
    }
}

void MainWindow::selectFolder(File* file) {
    folderTreeWidget->blockSignals(true);
    folderTreeWidget->clearSelection();
    folderTreeWidget->blockSignals(false);
    if (folderTreeWidget->isItemExist(file)) {
        auto folderItem = folderTreeWidget->getItem(file);
        folderItem->setSelected(true);
    } else {
        fileManager->expandFolders(file->getPath());
    }
}

void MainWindow::showImage(File* file) {
    viewWindow->setImage(file);
    if (isMaximized()) {
        viewWindow->showMaximized();
    } else {
        viewWindow->setGeometry(geometry());
        viewWindow->showNormal();
    }
    hide();
}
