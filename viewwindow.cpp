#include "viewwindow.h"
#include "mainwindow.h"
#include <QDebug>

ViewWindow::ViewWindow(QWidget* parent) : QMainWindow(parent) {
    mainWindow = static_cast<MainWindow*>(parent);
    QShortcut* escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escapePressed()));
    escapeShortcut->setAutoRepeat(false);
    viewWidget = new ViewWidget();
    setCentralWidget(viewWidget);
    setMouseTracking(true);
}

ViewWindow::~ViewWindow() {
}

QSize ViewWindow::sizeHint() const {
    return QSize(1280, 720);
}

void ViewWindow::closeEvent(QCloseEvent* event __attribute__((unused))) {
    showMainWindow();
}

void ViewWindow::wheelEvent(QWheelEvent* event) {
    if (event->delta() < 0) {
        nextImage();
    } else {
        prevImage();
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
    viewWidget->setImage(imageList.at(currentIndex));
}

void ViewWindow::escapePressed() {
    showMainWindow();
}

void ViewWindow::showMainWindow() {
    hide();
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
