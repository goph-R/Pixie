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

    MainWindow mainWindow;
    ViewWindow viewWindow(&mainWindow);
    mainWindow.setViewWindow(&viewWindow);

    if (argc > 1) {
        mainWindow.startWith(argv[1]);
        viewWindow.setEscapeQuits(true);
        viewWindow.show();
    } else {
        mainWindow.show();
    }

    return app.exec();
}
