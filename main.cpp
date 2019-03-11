#include <QCoreApplication>
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QDebug>
#include "domain/file.h"
#include "domain/foundfile.h"
#include "domain/foundfolder.h"
#include "ui/mainwindow.h"
#include "ui/viewwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Dynart");
    QCoreApplication::setApplicationName("Pixie");

    QApplication app(argc, argv);

    qRegisterMetaType<File*>("File*");
    qRegisterMetaType<FoundFile>("FoundFile");
    qRegisterMetaType<FoundFolder>("FoundFolder");

    const char* startPath = nullptr;
    if (argc > 1) {
        startPath = argv[1];
    }

    MainWindow mainWindow(startPath);
    ViewWindow viewWindow(&mainWindow);
    mainWindow.setViewWindow(&viewWindow);

    if (startPath != nullptr) {
        viewWindow.setCloseQuits(true);
        viewWindow.show();
    } else {
        mainWindow.show();
    }

    return app.exec();
}
