#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include <QMouseEvent>

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    ViewWidget(QWidget* parent=nullptr);
    void setImage(QString path);
    bool isFit();
    void setFit(bool value);

signals:
    void doubleClickedSignal();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QBrush backgroundBrush;
    QPixmap pixmap;
    bool fit;
    bool mouseDown;
    QPoint mouseDownPosition;
    QPoint translate;
};

#endif // VIEWWIDGET_H
