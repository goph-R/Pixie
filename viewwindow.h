#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include <QMainWindow>
#include <QShortcut>
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
    virtual void closeEvent(QCloseEvent* event) override;
    void setImage(File* file);

public slots:
    void escapePressed();

private:
    MainWindow* mainWindow;
    void showMainWindow();
    ViewWidget* viewWidget;
};

#endif // VIEWWINDOW_H
