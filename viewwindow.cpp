#include "viewwindow.h"
#include "mainwindow.h"
#include <QDebug>

ViewWindow::ViewWindow(QWidget* parent) : QMainWindow(parent) {
    mainWindow = static_cast<MainWindow*>(parent);

    fileManager = mainWindow->getFileManager();
    connect(fileManager, &FileManager::imageLoaded, this, &ViewWindow::imageLoaded);

    fileListWidget = mainWindow->getFileListWidget();

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(showMainWindow()));
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(switchFullscreen()));
    new QShortcut(QKeySequence("*"), this, SLOT(switchFit()));
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(prevImage()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(nextImage()));

    viewWidget = new ViewWidget();
    connect(viewWidget, &ViewWidget::doubleClickedSignal, this, &ViewWindow::showMainWindow);
    setCentralWidget(viewWidget);

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
    if (isMaximized()) { // TODO: when back from fullscreen this never true
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
    viewWidget->reset();
    viewWidget->setImage(image);
}

void ViewWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return) {
        showMainWindow();
    }
}
