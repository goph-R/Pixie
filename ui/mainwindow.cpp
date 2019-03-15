#include "mainwindow.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
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
#include <QFileInfo>
#include <QDir>
#include <QShortcut>
#include <QScrollBar>

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

MainWindow::MainWindow(Pixie* pixie) : QMainWindow(nullptr) {
    this->pixie = pixie;
}

MainWindow::~MainWindow() {
}

void MainWindow::create() {
    QSettings settings;
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());

    currentFolder = nullptr;
    fileToSelect = nullptr;
    startPath = pixie->getStartPath();
    config = pixie->getConfig();

    fileManager = pixie->getFileManager();
    QObject::connect(fileManager, SIGNAL(fileAdded(File*)), this, SLOT(addFile(File*)));
    QObject::connect(fileManager, SIGNAL(findFilesDone()), this, SLOT(findFilesDone()));
    QObject::connect(fileManager, SIGNAL(expandFoldersDone(QString)), this, SLOT(expandFoldersDone(QString)));

    thumbnailQueue = pixie->getThumbnailQueue();
    QObject::connect(thumbnailQueue, SIGNAL(doneSignal(QString, QImage)), this, SLOT(thumbnailDone(QString, QImage)));
    QObject::connect(thumbnailQueue, SIGNAL(errorSignal(QString)), this, SLOT(thumbnailError(QString)));
    QObject::connect(thumbnailQueue, SIGNAL(emptySignal(QString)), this, SLOT(thumbnailEmpty(QString)));

    viewWindow = pixie->getViewWindow();

    createUi();
    readSettings();

    if (startPath != nullptr) {
        startWith(startPath);
        setUiEnabled(false);
    }
}

void MainWindow::createUi() {
    setWindowTitle("Pixie");

    folderTreeWidget = new FolderTreeWidget();
    addDrives();
    QObject::connect(folderTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(folderExpanded(QTreeWidgetItem*)));
    QObject::connect(folderTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(folderSelectionChanged()));
    QObject::connect(fileManager, SIGNAL(folderAdded(File*)), folderTreeWidget, SLOT(addFolder(File*)));
    QObject::connect(fileManager, SIGNAL(folderEmpty(File*)), folderTreeWidget, SLOT(removeLoadingText(File*)));

    fileListWidget = new FileListWidget(config);
    QObject::connect(fileListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(execute(QListWidgetItem*)));
    QObject::connect(fileListWidget, SIGNAL(backspacePressed()), this, SLOT(backspacePressed()));
    QObject::connect(fileListWidget, SIGNAL(enterPressed()), this, SLOT(enterPressed()));
    QObject::connect(fileListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(fileSelectionChanged()));

    pathEdit = new QLineEdit();

    settingsDialog = new SettingsDialog(this);

    createMenu();
    createLayout();
}

void MainWindow::createMenu() {
    auto menu = menuBar();
    auto fileMenu = menu->addMenu("&File");
    auto quitAction = new QAction("&Quit", fileMenu);
    fileMenu->addAction(quitAction);
    QObject::connect(quitAction, SIGNAL(triggered()), this, SLOT(quitSlot()));

    auto toolsMenu = menu->addMenu("&Tools");
    auto settingsAction = new QAction("&Settings", toolsMenu);
    toolsMenu->addAction(settingsAction);
    QObject::connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

    auto helpMenu = menu->addMenu("&Help");
    auto aboutAction = new QAction("&About", helpMenu);
    helpMenu->addAction(aboutAction);
    QObject::connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

    new QShortcut(QKeySequence("Ctrl+V"), this, SLOT(pasteFiles()));
}

void MainWindow::createLayout() {
    dockWidget = new QDockWidget("Folders");
    dockWidget->setObjectName("foldersDockWidget");
    dockWidget->setWidget(folderTreeWidget);
    dockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    auto w = new QWidget();
    auto v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(4);
    v->addWidget(pathEdit);
    v->addWidget(fileListWidget);
    setCentralWidget(w);
}

QSize MainWindow::sizeHint() const {
    return QSize(1280, 720);
}

void MainWindow::setViewWindow(ViewWindow* value) {
    viewWindow = value;
}

void MainWindow::expandFoldersDone(QString path) {
    setUiEnabled(true);
    auto file = fileManager->getFileByPath(path);
    if (file == nullptr) {
        return;
    }
    folderTreeWidget->expandTo(file);
    folderTreeWidget->select(file);    
}

void MainWindow::closeEvent(QCloseEvent* event __attribute__((unused))) {
    pixie->exit();
}

void MainWindow::quitSlot() {
    pixie->exit();
}

void MainWindow::exit() {
    saveSettings();
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
    findFilesInCurrentFolder();
}

void MainWindow::findFilesInCurrentFolder() {
    bool addDotDotFolder = currentFolder->getParent()->getName() != "";
    fileManager->findFiles(currentFolder, addDotDotFolder);
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

void MainWindow::findFilesDone() {
    if (fileToSelect != nullptr) {
        fileListWidget->select(fileToSelect->getPath());
    }
    fileToSelect = nullptr;
    int imageCount = fileListWidget->countImages();
    int folderCount = fileListWidget->countFolders();
    QString text = tr("Images: %1  |  Folders: %2").arg(imageCount).arg(folderCount);
    statusBar()->showMessage(text);
    if (filePathToExecute == "") {
        return;
    }
    // this only runs if started with a filename
    if (fileListWidget->hasItem(filePathToExecute)) {
        QList<QListWidgetItem*> items;
        items.append(fileListWidget->getItem(filePathToExecute));
        showImage(items);
    }
    filePathToExecute = "";
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

void MainWindow::thumbnailEmpty(QString path) {
    fileListWidget->clearPixmap(path);
}

void MainWindow::backspacePressed() {
    goToParentFolder();
}

void MainWindow::goToParentFolder() {
    auto parent = currentFolder->getParent();
    if (parent->getPath() != "") {
        fileToSelect = currentFolder;
        folderTreeWidget->clearSelection();
        folderTreeWidget->select(parent);
    }
}

void MainWindow::enterPressed() {
    auto items = fileListWidget->selectedItems();
    if (items.size() == 1) {
        execute(items.at(0));
    } else {
        showImage(items);
    }
}

void MainWindow::pasteFiles() {
    auto clipboard = QApplication::clipboard();
    auto mimeData = clipboard->mimeData();
    if (!mimeData->hasUrls()) {
        return;
    }
    foreach (auto url, mimeData->urls()) {
        if (url.isLocalFile()) {
            pasteFile(url.toLocalFile());
        }
    }
    clipboard->clear();
    findFilesInCurrentFolder();
}

void MainWindow::setUiEnabled(bool value) {
    setEnabled(value);
}

void MainWindow::startWith(const char* p) {
    QString path = p;
    path.replace("\\", "/");
    path = path.replace(0, 1, path[0].toUpper());
    QFileInfo info(path);
    if (!info.isDir()) {
        filePathToExecute = path;
        path = info.absoluteDir().path() + "/";
    }
    fileManager->expandFolders(path);
}

void MainWindow::readSettings() {
    QSettings settings;
    restoreState(settings.value("mainWindowState").toByteArray());
    QString lastFolderPath = settings.value("lastFolderPath", "").toString();
    if (startPath == nullptr && lastFolderPath != "") {
        fileManager->expandFolders(lastFolderPath);
    }
}

void MainWindow::saveSettings() {
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
    if (currentFolder != nullptr) {
        settings.setValue("lastFolderPath", currentFolder->getPath());
    }
}

void MainWindow::addDrives() {
    auto icon = QIcon(":/icons/hard-drive.png");
    auto root = fileManager->getRoot();
    foreach (auto file, root->getChildren()) {
        auto item = folderTreeWidget->createItem(nullptr, file, icon);
        folderTreeWidget->addTopLevelItem(item);
    }
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
    if (file->getParent() != nullptr && file->getParent()->getPath() != currentFolder->getPath()) {
        return;
    }
    if (fileListWidget->hasItem(file->getPath())) {
        return;
    }
    fileListWidget->createItem(file);
    if (!file->isDotDot() && (file->isFolder() || file->isImage())) {
        thumbnailQueue->add(file);
    }
}

void MainWindow::enterFolder(File* file) {
    if (file->isDotDot()) {
        goToParentFolder();
    } else {
        folderTreeWidget->clearSelection();
        folderTreeWidget->expandTo(file);
        folderTreeWidget->select(file);
    }
}

void MainWindow::showImage(QList<QListWidgetItem*> items) {
    QString path = "";
    QStringList paths;
    QList<File*> files;
    bool changeSelection = true;
    if (items.size() == 1) {
        auto fileItem = static_cast<FileListItem*>(items.at(0));
        auto file = fileItem->getFile();
        path = file->getPath();
        files = currentFolder->getChildren();
    } else {
        changeSelection = false;
        foreach (auto item, items) {
            auto fileItem = static_cast<FileListItem*>(item);
            files.append(fileItem->getFile());
        }
    }
    foreach (auto file, files) {
        if (!file->isImage()) {
            continue;
        }
        if (path == "") {
            path = file->getPath();
        }
        paths.append(file->getPath());
    }
    if (paths.size() < 1) {
        return;
    }
    viewWindow->setImages(path, paths, changeSelection);
    if (isMaximized()) {
        viewWindow->showMaximized();
    } else {
        viewWindow->showNormal();
        viewWindow->setGeometry(geometry());
    }
    hide();
}

void MainWindow::execute(QListWidgetItem* item) {
    auto fileItem = static_cast<FileListItem*>(item);
    auto file = fileItem->getFile();
    if (file->isFolder()) {
        enterFolder(file);
    } else if (file->isImage()) {
        QList<QListWidgetItem*> items;
        items.append(item);
        showImage(items);
    }
}

void MainWindow::pasteFile(QString srcPath) {
    QFileInfo info(srcPath);
    if (info.isDir()) {
        // TODO
        return;
    }
    QString dstPath = currentFolder->getPath() + info.fileName();
    QFileInfo dstInfo(dstPath);
    if (dstInfo.exists()) {
        // TODO
        return;
    }
    QFile::copy(srcPath, dstPath);
}
