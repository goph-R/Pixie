#include "viewwindow.h"
#include "mainwindow.h"
#include <QDebug>

ViewWindow::ViewWindow(QWidget* parent) : QMainWindow(parent) {
    mainWindow = static_cast<MainWindow*>(parent);
    QShortcut* escapeShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escapePressed()));
    escapeShortcut->setAutoRepeat(false);
    viewWidget = new ViewWidget();
    setCentralWidget(viewWidget);
}

ViewWindow::~ViewWindow() {
}

QSize ViewWindow::sizeHint() const {
    return QSize(1280, 720);
}

void ViewWindow::closeEvent(QCloseEvent* event __attribute__((unused))) {
    showMainWindow();
}

void ViewWindow::escapePressed() {
    showMainWindow();
}

void ViewWindow::showMainWindow() {
    if (isMaximized()) {
        mainWindow->showMaximized();
    } else {
        mainWindow->setGeometry(geometry());
        mainWindow->showNormal();
    }
    hide();
}

void ViewWindow::setImage(File* file) {
    viewWidget->setImage(file->getPath());
}
