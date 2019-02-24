#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include "domain/file.h"
#include "domain/foundfile.h"
#include "domain/foundfolder.h"
#include "ui/mainwindow.h"
#include "ui/viewwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<File*>("File*");
    qRegisterMetaType<FoundFile>("FoundFile");
    qRegisterMetaType<FoundFolder>("FoundFolder");

    MainWindow mainWindow;
    ViewWindow viewWindow(&mainWindow);
    mainWindow.setViewWindow(&viewWindow);

    QMainWindow* active = &mainWindow; // TODO: open the image view if filename argument presents

#if QT_VERSION >= 0x050000
    QRect availableGeometry = app.screens().at(app.desktop()->screenNumber())->availableGeometry();
#else
    QRect availableGeometry = QApplication::desktop()->screenGeometry();
#endif
    active->show();
    active->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, active->size(), availableGeometry));

    return app.exec();
}
