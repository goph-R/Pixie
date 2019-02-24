#include "mainwindow.h"

#include <QPixmap>
#include <QDebug>
#include <QVBoxLayout>
#include <QLineEdit>

#include "domain/filemanager.h"
#include "domain/config.h"
#include "domain/thumbnailqueue.h"
#include "domain/file.h"
#include "ui/foldertreewidget.h"
#include "ui/foldertreeitem.h"
#include "ui/filelistwidget.h"
#include "ui/filelistitem.h"
#include "ui/viewwindow.h"

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
    v->setSpacing(4);
    v->addWidget(pathEdit);
    v->addWidget(fileListWidget);

    setCentralWidget(w);

    QObject::connect(folderTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(folderExpanded(QTreeWidgetItem*)));
    QObject::connect(folderTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(folderSelectionChanged()));
    QObject::connect(fileManager, SIGNAL(folderAdded(File*)), folderTreeWidget, SLOT(addFolder(File*)));
    QObject::connect(fileManager, SIGNAL(folderEmpty(File*)), folderTreeWidget, SLOT(removeLoadingText(File*)));
    QObject::connect(fileManager, SIGNAL(fileAdded(File*)), this, SLOT(addFile(File*)));
    QObject::connect(fileManager, SIGNAL(findFilesDone()), this, SLOT(findFilesDone()));
    QObject::connect(thumbnailQueue, SIGNAL(done(QString, QImage)), this, SLOT(thumbnailDone(QString, QImage)));
    QObject::connect(thumbnailQueue, SIGNAL(error(QString)), this, SLOT(thumbnailError(QString)));
    QObject::connect(fileListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(execute(QListWidgetItem*)));
    QObject::connect(fileListWidget, SIGNAL(backspacePressed()), this, SLOT(backspacePressed()));
    QObject::connect(fileListWidget, SIGNAL(enterPressed()), this, SLOT(enterPressed()));
    QObject::connect(fileListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(fileSelectionChanged()));
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
    setPathEditTo(currentFolder->getPath());
    fileManager->findFiles(currentFolder);
}

void MainWindow::setPathEditTo(QString path) {
    if (config->useBackslash()) {
        path = path.replace("/", "\\");
    }
    pathEdit->setText(path);
}

void MainWindow::fileSelectionChanged() {
    auto selectedItems = fileListWidget->selectedItems();
    if (selectedItems.size() != 1) {
        return;
    }
    auto fileItem = static_cast<FileListItem*>(selectedItems.at(0));
    auto file = fileItem->getFile();
    setPathEditTo(file->getPath());
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
