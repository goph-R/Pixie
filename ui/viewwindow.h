#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QThread>
#include "pixie.h"

class File;
class ViewWidget;
class FileListWidget;
class MainWindow;
class ImageWorker;

class ViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    ViewWindow(Pixie* pixie);
    virtual ~ViewWindow() override;
    void create();
    virtual QSize sizeHint() const override;
    void setImages(QString path, QStringList paths, bool changeSelection);
    void exit();
    void setCloseQuits(bool value);

public slots:
    void showMainWindow();
    void switchFullscreen();
    void switchFit();
    void imageLoaded(const QString path, const QRect rect, const QImage image);
    void imageDone(const QString path);
    void escapePressed();
    void leftPressed();
    void rightPressed();
    void upPressed();
    void downPressed();
    void plusPressed();
    void minusPressed();
    void showContextMenu(const QPoint &pos);
    void copyToTriggered();
    void copyToLastTriggered();
    void rotateLeftTriggered();
    void rotateRightTriggered();

signals:
    void loadImage(QString path);

protected:
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;    
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void changeEvent(QEvent* event) override;

private:
    void createImageWorkerThread();
    void createUi();
    void createShortcuts();
    void readSettings();
    void saveSettings();
    void nextImage();
    void prevImage();
    void loadCurrentImage();
    void goFullscreen();
    void backFromFullscreen();
    const QString getCurrentPath();
    void copyToPath(QString path);
    Pixie* pixie;
    QThread imageWorkerThread;
    QStringList imageList;
    QImage emptyImage;
    QString lastCopyToPath;
    MainWindow* mainWindow;
    ViewWidget* viewWidget;
    FileListWidget* fileListWidget;
    ImageWorker* imageWorker;
    int currentIndex;
    bool wasMaximized;
    bool closeQuits;
    bool changeSelection;

};

#endif // VIEWWINDOW_H
