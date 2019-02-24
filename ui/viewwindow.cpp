#include "viewwindow.h"

#include <QShortcut>
#include <QTimer>
#include <QImage>
#include "domain/file.h"
#include "domain/filemanager.h"
#include "ui/mainwindow.h"
#include "ui/viewwidget.h"
#include "ui/filelistwidget.h"

#include <QDebug>

ViewWindow::ViewWindow(MainWindow *mainWindow, QWidget* parent) : QMainWindow(parent) {
    this->mainWindow = mainWindow;

    fileManager = mainWindow->getFileManager();
    fileListWidget = mainWindow->getFileListWidget();

    new QShortcut(QKeySequence("*"), this, SLOT(switchFit()));
    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escapePressed()));
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(switchFullscreen()));
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(leftPressed()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(rightPressed()));
    new QShortcut(QKeySequence(Qt::Key_Up), this, SLOT(upPressed()));
    new QShortcut(QKeySequence(Qt::Key_Down), this, SLOT(downPressed()));
    new QShortcut(QKeySequence(Qt::Key_Plus), this, SLOT(plusPressed()));
    new QShortcut(QKeySequence(Qt::Key_Minus), this, SLOT(minusPressed()));

    viewWidget = new ViewWidget();
    setCentralWidget(viewWidget);

    QObject::connect(viewWidget, SIGNAL(doubleClickedSignal()), this, SLOT(showMainWindow()));
    QObject::connect(fileManager, SIGNAL(imageLoaded(QImage)), this, SLOT(imageLoaded(QImage)));

    wasMaximized = false;
}

ViewWindow::~ViewWindow() {
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
    viewWidget->setImage(emptyImage);
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
    }
    loadCurrentImage();
}

void ViewWindow::prevImage() {
    currentIndex--;
    if (currentIndex < 0) {
        currentIndex = 0;
    }
    loadCurrentImage();
}

void ViewWindow::loadCurrentImage() {
    QString path = imageList.at(currentIndex);
    fileManager->loadImage(path);
    fileListWidget->select(path);
}

void ViewWindow::imageLoaded(const QImage image) {
    viewWidget->setImage(image);
}

void ViewWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return) {
        showMainWindow();
    }
}
