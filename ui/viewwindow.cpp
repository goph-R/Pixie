#include "viewwindow.h"

#include <QApplication>
#include <QShortcut>
#include <QImage>
#include <QImageReader>
#include <QPainter>
#include "domain/file.h"
#include "domain/imageworker.h"
#include "ui/mainwindow.h"
#include "ui/viewwidget.h"
#include "ui/filelistwidget.h"

#include <QDebug>

ViewWindow::ViewWindow(MainWindow *mainWindow, QWidget* parent) : QMainWindow(parent) {
    createImageWorkerThread();

    this->mainWindow = mainWindow;
    fileListWidget = mainWindow->getFileListWidget();
    viewWidget = new ViewWidget();
    setCentralWidget(viewWidget);

    new QShortcut(QKeySequence("*"), this, SLOT(switchFit()));
    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escapePressed()));
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(switchFullscreen()));
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(leftPressed()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(rightPressed()));
    new QShortcut(QKeySequence(Qt::Key_Up), this, SLOT(upPressed()));
    new QShortcut(QKeySequence(Qt::Key_Down), this, SLOT(downPressed()));
    new QShortcut(QKeySequence(Qt::Key_Plus), this, SLOT(plusPressed()));
    new QShortcut(QKeySequence(Qt::Key_Minus), this, SLOT(minusPressed()));

    QObject::connect(viewWidget, SIGNAL(doubleClickedSignal()), this, SLOT(showMainWindow()));

    wasMaximized = false;
}

ViewWindow::~ViewWindow() {
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

void ViewWindow::exitApplication() {
    imageWorkerThread.quit();
    imageWorkerThread.wait();
}

void ViewWindow::setMaximized(bool value) {
    wasMaximized = value;
    if (value) {
        showMaximized();
    } else {
        showNormal();
    }
}

QSize ViewWindow::sizeHint() const {
    return QSize(1280, 720);
}

void ViewWindow::wheelEvent(QWheelEvent* event) {
    if (event->delta() < 0) {
        nextImage();
    } else {
        prevImage();
    }
}

void ViewWindow::closeEvent(QCloseEvent* event __attribute__((unused))) {
    showMainWindow();
}

void ViewWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return) {
        showMainWindow();
    }
}

void ViewWindow::switchFit() {
    viewWidget->setFit(!viewWidget->isFit());
}

void ViewWindow::escapePressed() {
    // TODO: if started from CLI close the app
    showMainWindow();
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

void ViewWindow::setImage(File* file) {
    viewWidget->setPixmap(nullptr);
    fillImageList(file->getParent());
    currentIndex = imageList.indexOf(file->getPath());
    loadCurrentImage();
}

void ViewWindow::fillImageList(File* parent) {
    imageList.clear();
    foreach (auto child, parent->getChildren()) {
        if (child->isImage()) {
            imageList.append(child->getPath());
        }
    }
}

void ViewWindow::showMainWindow() {
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
        wasMaximized = isMaximized();
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
    QPixmap* pixmap = new QPixmap(reader.size());
    pixmap->fill(QColor(24, 24, 24));
    viewWidget->setPixmap(pixmap, false);
    imageWorker->setPathToLoad(path);
    emit loadImage(path);
    fileListWidget->select(path);
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

