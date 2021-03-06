#include "viewwindow.h"

#include <QApplication>
#include <QShortcut>
#include <QImage>
#include <QImageReader>
#include <QPainter>
#include <QImageIOHandler>
#include <QSettings>
#include "domain/file.h"
#include "domain/imageworker.h"
#include "domain/imageutils.h"
#include "ui/mainwindow.h"
#include "ui/viewwidget.h"
#include "ui/filelistwidget.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMenu>

ViewWindow::ViewWindow(Pixie* pixie) : QMainWindow(nullptr) {
    this->pixie = pixie;
}

ViewWindow::~ViewWindow() {

}

void ViewWindow::create() {
    mainWindow = pixie->getMainWindow();
    wasMaximized = false;
    closeQuits = false;
    lastCopyToPath = QDir::homePath();

    QSettings settings;
    restoreGeometry(settings.value("viewWindowGeometry").toByteArray());

    createImageWorkerThread();
    createUi();
    createShortcuts();

    readSettings();
}

QSize ViewWindow::sizeHint() const {
    return QSize(1280, 720);
}

void ViewWindow::setCloseQuits(bool value) {
    closeQuits = value;
}

void ViewWindow::exit() {
    saveSettings();
    imageWorkerThread.quit();
    imageWorkerThread.wait();
}

void ViewWindow::setImages(QString path, QStringList paths, bool changeSelection) {
    this->changeSelection = changeSelection;
    viewWidget->setPixmap(nullptr);
    imageList = paths;
    currentIndex = imageList.indexOf(path);
    loadCurrentImage();
}

void ViewWindow::wheelEvent(QWheelEvent* event) {
    if (event->delta() < 0) {
        nextImage();
    } else {
        prevImage();
    }
}

void ViewWindow::closeEvent(QCloseEvent* event __attribute__((unused))) {
    if (closeQuits) {
        pixie->exit();
    } else {
        showMainWindow();
    }
}

void ViewWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return) {
        showMainWindow();
    }
}

void ViewWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        auto stateChangeEvent = static_cast<QWindowStateChangeEvent*>(event);
        wasMaximized = stateChangeEvent->oldState() & Qt::WindowMaximized;
    }
    QMainWindow::changeEvent(event);
}

void ViewWindow::createUi() {
    fileListWidget = mainWindow->getFileListWidget();
    viewWidget = new ViewWidget();
    setCentralWidget(viewWidget);
    QObject::connect(viewWidget, SIGNAL(doubleClickedSignal()), this, SLOT(showMainWindow()));
    QObject::connect(viewWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

}

void ViewWindow::createShortcuts() {
    new QShortcut(QKeySequence("*"), this, SLOT(switchFit()));
    new QShortcut(QKeySequence("0"), this, SLOT(switchFit()));
    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escapePressed()));
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(switchFullscreen()));
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(leftPressed()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(rightPressed()));
    new QShortcut(QKeySequence(Qt::Key_Up), this, SLOT(upPressed()));
    new QShortcut(QKeySequence(Qt::Key_Down), this, SLOT(downPressed()));
    new QShortcut(QKeySequence(Qt::Key_Plus), this, SLOT(plusPressed()));
    new QShortcut(QKeySequence(Qt::Key_Minus), this, SLOT(minusPressed()));
}

void ViewWindow::createImageWorkerThread() {
    imageWorker = new ImageWorker();
    imageWorker->moveToThread(&imageWorkerThread);
    QObject::connect(this, SIGNAL(loadImage(QString)), imageWorker, SLOT(load(QString)));
    QObject::connect(imageWorker, SIGNAL(loaded(const QString, const QRect, const QImage)), this, SLOT(imageLoaded(const QString, const QRect, const QImage)));
    QObject::connect(imageWorker, SIGNAL(done(const QString)), this, SLOT(imageDone(const QString)));
    QObject::connect(&imageWorkerThread, SIGNAL(finished()), imageWorker, SLOT(deleteLater()));
    imageWorkerThread.start();
}

void ViewWindow::readSettings() {
    QSettings settings;
    restoreState(settings.value("viewWindowState").toByteArray());
    lastCopyToPath = settings.value("lastCopyToPath", lastCopyToPath).toString();
}

void ViewWindow::saveSettings() {
    QSettings settings;
    settings.setValue("viewWindowGeometry", saveGeometry());
    settings.setValue("viewWindowState", saveState());
    settings.setValue("lastCopyToPath", lastCopyToPath);
}

void ViewWindow::switchFit() {
    viewWidget->setFit(!viewWidget->isFit());
}

void ViewWindow::escapePressed() {
    if (closeQuits) {
        mainWindow->exit();
    } else {
        showMainWindow();
    }
}

void ViewWindow::leftPressed() {
    // TODO: settings to translate/prev image
    prevImage();
}

void ViewWindow::rightPressed() {
    // TODO: settings to translate/next image
    nextImage();
}

void ViewWindow::upPressed() {
    viewWidget->translateUp();
}

void ViewWindow::downPressed() {
    viewWidget->translateDown();
}

void ViewWindow::minusPressed() {
    viewWidget->zoomOut();
}

void ViewWindow::plusPressed() {
    viewWidget->zoomIn();
}

void ViewWindow::showMainWindow() {
    closeQuits = false;
    backFromFullscreen();
    if (isMaximized()) { // FIX: when back from fullscreen this never true
        mainWindow->showMaximized();
    } else {
        mainWindow->showNormal();
        mainWindow->setGeometry(geometry());
    }
    hide();
}

void ViewWindow::switchFullscreen() {
    if (isFullScreen()) {
        backFromFullscreen();
    } else {
        goFullscreen();
    }
}

void ViewWindow::goFullscreen() {
    if (!isFullScreen()) {
        showFullScreen();
    }
}

void ViewWindow::backFromFullscreen() {
    if (isFullScreen()) {
        if (wasMaximized) {
            showMaximized();
        } else {
            showNormal();
        }
    }
}

void ViewWindow::nextImage() {
    currentIndex++;
    if (currentIndex >= imageList.size()) {
        currentIndex = imageList.size() - 1;
        return;
    }
    loadCurrentImage();
}

void ViewWindow::prevImage() {
    currentIndex--;
    if (currentIndex < 0) {
        currentIndex = 0;
        return;
    }
    loadCurrentImage();
}

const QString ViewWindow::getCurrentPath() {
    return imageList.at(currentIndex);
}

void ViewWindow::loadCurrentImage() {
    QString path = getCurrentPath();
    QImageReader reader(path);
    auto size = reader.size();
    size = ImageUtils::getTransformedSize(&reader, size);
    QPixmap* pixmap = new QPixmap(size);
    pixmap->fill(QColor(24, 24, 24));
    viewWidget->setPixmap(pixmap, false);
    imageWorker->setPathToLoad(path);
    emit loadImage(path);
    if (changeSelection) {
        fileListWidget->select(path);
    }
}

void ViewWindow::imageLoaded(const QString path, const QRect rect, const QImage image) {
    if (path != getCurrentPath()) {
        return;
    }
    QPixmap* pixmap = viewWidget->getPixmap();
    if (pixmap == nullptr) {
        return;
    }
    QPainter p(pixmap);
    p.drawImage(rect, image);
    viewWidget->update();
}

void ViewWindow::imageDone(const QString path) {
    if (path == getCurrentPath()) {
        viewWidget->setLoaded();
    }
}

void ViewWindow::showContextMenu(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);

    QAction copyToPathAction(&contextMenu);
    QAction copyToAction("Copy to ..", &contextMenu);
    QAction rotateLeftAction("Rotate left", &contextMenu);
    QAction rotateRightAction("Rotate right", &contextMenu);

    connect(&copyToPathAction, SIGNAL(triggered()), this, SLOT(copyToLastTriggered()));
    connect(&copyToAction, SIGNAL(triggered()), this, SLOT(copyToTriggered()));
    connect(&rotateLeftAction, SIGNAL(triggered()), this, SLOT(rotateLeftTriggered()));
    connect(&rotateRightAction, SIGNAL(triggered()), this, SLOT(rotateRightTriggered()));

    if (!lastCopyToPath.isEmpty()) {
        QFileInfo info(lastCopyToPath);
        copyToPathAction.setText("Copy to " + info.fileName());
        contextMenu.addAction(&copyToPathAction);
    }
    contextMenu.addAction(&copyToAction);
    contextMenu.addSeparator();
    contextMenu.addAction(&rotateLeftAction);
    contextMenu.addAction(&rotateRightAction);

    contextMenu.exec(mapToGlobal(pos));
}

void ViewWindow::copyToLastTriggered() {
    copyToPath(lastCopyToPath);
}

void ViewWindow::copyToTriggered() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Copy to .."), lastCopyToPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty()) {
        return;
    }
    copyToPath(path);
}

void ViewWindow::rotateLeftTriggered() {
    viewWidget->rotate(-90);
}

void ViewWindow::rotateRightTriggered() {
    viewWidget->rotate(90);
}

void ViewWindow::copyToPath(QString path) {
    if (path.endsWith("/")) {
        path = path.left(path.length() - 1);
    }
    QFileInfo info(getCurrentPath());
    QFile::copy(getCurrentPath(), path + "/" + info.fileName());
    lastCopyToPath = path;
}
