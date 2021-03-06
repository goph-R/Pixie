#include "pixie.h"

#include "domain/file.h"
#include "domain/foundfile.h"
#include "domain/foundfolder.h"
#include "domain/filemanager.h"
#include "domain/config.h"
#include "domain/thumbnailqueue.h"
#include "ui/theme.h"
#include "ui/mainwindow.h"
#include "ui/viewwindow.h"

Pixie::Pixie(int argc, char** argv) {
    QCoreApplication::setOrganizationName("Dynart");
    QCoreApplication::setApplicationName("Pixie");

    qRegisterMetaType<File*>("File*");
    qRegisterMetaType<FoundFile>("FoundFile");
    qRegisterMetaType<FoundFolder>("FoundFolder");

    startPath = nullptr;
    if (argc > 1) {
        startPath = argv[1];
    }

    app = new QApplication(argc, argv);

    config = new Config();
    theme = new Theme(config);
    fileManager = new FileManager(config);
    thumbnailQueue = new ThumbnailQueue(config, fileManager);

    mainWindow = new MainWindow(this);
    viewWindow = new ViewWindow(this);
    mainWindow->create();
    viewWindow->create();

    theme->apply(mainWindow);
    theme->apply(viewWindow);
}

Pixie::~Pixie() {
}

int Pixie::run() {
    if (startPath != nullptr) {
        viewWindow->setCloseQuits(true);
        viewWindow->show();
    } else {
        mainWindow->show();
    }
    return app->exec();
}

const char* Pixie::getStartPath() {
    return startPath;
}

MainWindow* Pixie::getMainWindow() {
    return mainWindow;
}

ViewWindow* Pixie::getViewWindow() {
    return viewWindow;
}

Config* Pixie::getConfig() {
    return config;
}

Theme* Pixie::getTheme() {
    return theme;
}

FileManager* Pixie::getFileManager() {
    return fileManager;
}

ThumbnailQueue* Pixie::getThumbnailQueue() {
    return thumbnailQueue;
}

void Pixie::exit() {
    QThreadPool::globalInstance()->waitForDone();
    mainWindow->exit();
    viewWindow->exit();

    delete thumbnailQueue;
    delete fileManager;
    delete theme;
    delete config;

    QApplication::quit();

    // Delete windows after quit because of a crash possibility
    delete viewWindow;
    delete mainWindow;

}
