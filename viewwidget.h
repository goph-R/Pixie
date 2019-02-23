#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include <QMouseEvent>

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    ViewWidget(QWidget* parent=nullptr);
    void reset();
    void setImage(const QImage image);
    bool isFit();
    void setFit(bool value);

public slots:
    void translateUp();
    void translateDown();
    void zoomIn();
    void zoomOut();

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
    QImage image;
    bool fit;
    bool mouseDown;
    QPoint mouseDownPosition;
    QPoint translate;
    int zoomLevel;
    const int zoomLevel100 = 13;
    QList<float> zoomLevels;
};

#endif // VIEWWIDGET_H
