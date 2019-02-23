#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include "mainwindow.h"
#include "viewwindow.h"
#include "file.h"
#include "foundfile.h"
#include "foundfolder.h"

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
