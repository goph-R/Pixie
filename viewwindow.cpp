#include "viewwindow.h"
#include "mainwindow.h"
#include <QDebug>

ViewWindow::ViewWindow(QWidget* parent) : QMainWindow(parent) {
    mainWindow = static_cast<MainWindow*>(parent);

    QShortcut* escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escapePressed()));
    escapeShortcut->setAutoRepeat(false);

    QShortcut* fullscreenShortcut = new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(switchFullscreen()));
    fullscreenShortcut->setAutoRepeat(false);

    viewWidget = new ViewWidget();
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

void ViewWindow::escapePressed() {
    showMainWindow();
}

void ViewWindow::mouseDoubleClickEvent(QMouseEvent *event __attribute__((unused))) {
    showMainWindow();
}

void ViewWindow::setImage(File* file) {
    fillImageList(file->getParent());
    currentIndex = imageList.indexOf(file->getPath());
    showCurrentImage();
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
    if (isMaximized()) { // TODO: when back from fullscreen this isn't true
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
    showCurrentImage();
}

void ViewWindow::prevImage() {
    currentIndex--;
    if (currentIndex < 0) {
        currentIndex = 0;
    }
    showCurrentImage();
}

void ViewWindow::showCurrentImage() {
    QString path = imageList.at(currentIndex);
    viewWidget->setImage(path);
}
