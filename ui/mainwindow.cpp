#include "mainwindow.h"

#include <QApplication>
#include <QPixmap>
#include <QDebug>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QSizePolicy>
#include <QSplitter>
#include <QSettings>
#include <QThreadPool>
#include <QStatusBar>

#include "domain/filemanager.h"
#include "domain/config.h"
#include "domain/thumbnailqueue.h"
#include "domain/file.h"
#include "domain/imageworker.h"
#include "ui/foldertreewidget.h"
#include "ui/foldertreeitem.h"
#include "ui/filelistwidget.h"
#include "ui/filelistitem.h"
#include "ui/viewwindow.h"
#include "ui/settingsdialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    QSettings settings;
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());

    currentFolder = nullptr;
    fileToSelect = nullptr;

    setWindowTitle("Pixie");

    // domain
    config = new Config();
    fileManager = new FileManager(config);
    thumbnailQueue = new ThumbnailQueue(config, fileManager);

    // ui
    folderTreeWidget = new FolderTreeWidget();
    dockWidget = new QDockWidget("Folders");
    dockWidget->setObjectName("foldersDockWidget");
    dockWidget->setWidget(folderTreeWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    fileListWidget = new FileListWidget(config);
    pathEdit = new QLineEdit();
    settingsDialog = new SettingsDialog(this);

    addDrives();

    // central widget
    auto w = new QWidget();
    auto v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(4);
    v->addWidget(pathEdit);
    v->addWidget(fileListWidget);
    setCentralWidget(w);

    // menu
    auto menu = menuBar();
    auto fileMenu = menu->addMenu("&File");
    auto quitAction = new QAction("&Quit", fileMenu);
    fileMenu->addAction(quitAction);

    auto toolsMenu = menu->addMenu("&Tools");
    auto settingsAction = new QAction("&Settings", toolsMenu);
    toolsMenu->addAction(settingsAction);

    auto helpMenu = menu->addMenu("&Help");
    auto aboutAction = new QAction("&About", helpMenu);
    helpMenu->addAction(aboutAction);

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
    QObject::connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    QObject::connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
    QObject::connect(quitAction, SIGNAL(triggered()), this, SLOT(quitSlot()));

    readSettings();
}

MainWindow::~MainWindow() {
}

void MainWindow::setViewWindow(ViewWindow* value) {
    viewWindow = value;
}

void MainWindow::closeEvent(QCloseEvent* event __attribute__((unused))) {
    exitApplication();
}

void MainWindow::quitSlot() {
    exitApplication();
}

void MainWindow::exitApplication() {
    saveSettings();
    viewWindow->exitApplication();
    QThreadPool::globalInstance()->waitForDone();
    delete thumbnailQueue;
    delete fileManager;
    delete config;
    QApplication::quit();
}

void MainWindow::saveSettings() {
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
}

void MainWindow::readSettings() {
    QSettings settings;
    restoreState(settings.value("mainWindowState").toByteArray());
}

void MainWindow::showSettings() {
    settingsDialog->exec();
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "About Pixie", "<b>Pixie</b><br><br>The Image Manager.<br><small>Version: 0.1</small>");
}

FileManager* MainWindow::getFileManager() {
    return fileManager;
}

FileListWidget* MainWindow::getFileListWidget() {
    return fileListWidget;
}

void MainWindow::resizeEvent(QResizeEvent *event __attribute__((unused))) {
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

void MainWindow::fileSelectionChanged() {
    auto selectedItems = fileListWidget->selectedItems();
    if (selectedItems.size() != 1) {
        return;
    }
    auto fileItem = static_cast<FileListItem*>(selectedItems.at(0));
    auto file = fileItem->getFile();
    setPathEditTo(file->getPath());
}

void MainWindow::setPathEditTo(QString path) {
    if (config->useBackslash()) {
        path = path.replace("/", "\\");
    }
    pathEdit->setText(path);
    QString winTitle = "Pixie - " + path;
    setWindowTitle(winTitle);
    viewWindow->setWindowTitle(winTitle);
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
    int imageCount = fileListWidget->countImages();
    int folderCount = fileListWidget->countFolders();
    QString text = tr("Images: %1  |  Folders: %2").arg(imageCount).arg(folderCount);
    statusBar()->showMessage(text);
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
