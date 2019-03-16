#ifndef PIXIE_H
#define PIXIE_H

#include <QCoreApplication>
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QDebug>

class MainWindow;
class ViewWindow;
class Config;
class FileManager;
class ThumbnailQueue;
class Theme;

class Pixie
{
public:
    Pixie(int argc, char** argv);
    ~Pixie();
    int run();
    void exit();
    const char* getStartPath();
    MainWindow* getMainWindow();
    ViewWindow* getViewWindow();
    Config* getConfig();
    Theme* getTheme();
    FileManager* getFileManager();
    ThumbnailQueue* getThumbnailQueue();

private:
    QApplication* app;
    Config* config;
    FileManager* fileManager;
    ThumbnailQueue* thumbnailQueue;
    MainWindow* mainWindow;
    ViewWindow* viewWindow;
    Theme* theme;
    const char* startPath;
};

#endif // PIXIE_H
