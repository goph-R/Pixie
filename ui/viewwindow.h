#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include <QMainWindow>

class File;
class ViewWidget;
class FileManager;
class FileListWidget;
class MainWindow;

class ViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    ViewWindow(MainWindow* mainWindow, QWidget* parent=nullptr);
    virtual ~ViewWindow() override;
    virtual QSize sizeHint() const override;
    void setImage(File* file);
    void setMaximized(bool value);

public slots:
    void showMainWindow();
    void switchFullscreen();
    void switchFit();
    void imageLoaded(const QImage image);
    void escapePressed();
    void leftPressed();
    void rightPressed();
    void upPressed();
    void downPressed();
    void plusPressed();
    void minusPressed();
    void quit();

protected:
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;    
    virtual void keyPressEvent(QKeyEvent* event) override;

private:
    void nextImage();
    void prevImage();
    void loadCurrentImage();
    void fillImageList(File* parent);
    void goFullscreen();
    void backFromFullscreen();
    MainWindow* mainWindow;
    ViewWidget* viewWidget;
    QStringList imageList;
    int currentIndex;
    bool wasMaximized;
    FileManager* fileManager;
    FileListWidget* fileListWidget;
    QImage emptyImage;

};

#endif // VIEWWINDOW_H
