#include "mainwindow.h"
#include <QApplication>

#include <QScreen>
#include <QDesktopWidget>

#include "file.h"
#include "foundfile.h"
#include "foundfolder.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<File*>("File*");
    qRegisterMetaType<FoundFile>("FoundFile");
    qRegisterMetaType<FoundFile>("FoundFolder");

    MainWindow w;
    w.show();    

    // w.center() or something like that???
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), a.screens().at(a.desktop()->screenNumber())->availableGeometry()));

    return a.exec();
}
