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

    MainWindow mainWindow;
    mainWindow.show();
    mainWindow.setGeometry(QStyle::alignedRect(
        Qt::LeftToRight, Qt::AlignCenter, mainWindow.size(),
        app.screens().at(app.desktop()->screenNumber())->availableGeometry())
    );

    return app.exec();
}
