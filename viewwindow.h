#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QTimer>
#include "viewwidget.h"
#include "file.h"

class MainWindow;

class ViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    ViewWindow(QWidget* parent=nullptr);
    virtual ~ViewWindow() override;
    virtual QSize sizeHint() const override;
    void setImage(File* file);
    void setMaximized(bool value);

public slots:
    void escapePressed();
    void switchFullscreen();

protected:
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void nextImage();
    void prevImage();
    void showCurrentImage();
    void fillImageList(File* parent);
    MainWindow* mainWindow;
    void showMainWindow();
    ViewWidget* viewWidget;
    QStringList imageList;
    int currentIndex;
    bool wasMaximized;
    void goFullscreen();
    void backFromFullscreen();
};

#endif // VIEWWINDOW_H
