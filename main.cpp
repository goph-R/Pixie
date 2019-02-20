#include "mainwindow.h"
#include <QApplication>

#include <QScreen>
#include <QDesktopWidget>

#include "file.h"
#include "foundfile.h"
#include "foundfolder.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<File*>("File*");
    qRegisterMetaType<FoundFile>("FoundFile");
    qRegisterMetaType<FoundFile>("FoundFolder");

    auto config = new Config();
    auto fileManager = new FileManager();
    auto thumbnailQueue = new ThumbnailQueue(config);

    MainWindow mainWindow(config, fileManager, thumbnailQueue);
    mainWindow.show();
    mainWindow.setGeometry(QStyle::alignedRect(
        Qt::LeftToRight, Qt::AlignCenter, mainWindow.size(),
        app.screens().at(app.desktop()->screenNumber())->availableGeometry())
    );

    int result = app.exec();

    delete thumbnailQueue;
    delete fileManager;
    delete config;

    return result;
}
