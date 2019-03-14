#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    ViewWidget(QWidget* parent=nullptr);
    void setPixmap(QPixmap* pixmap, bool update=true);
    QPixmap* getPixmap();
    bool isFit();
    void setFit(bool value);
    void translateUp();
    void translateDown();
    void zoomIn();
    void zoomOut();
    void setLoaded();
    void rotate(int angle);

signals:
    void doubleClickedSignal();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    const int ZOOM_ORIGINAL = 11;

    void limitTranslate();
    void calculateDrawSize();
    void calculateFittedDrawSize();
    void calculateZoomedDrawSize();
    void drawBackground(QPainter &painter);
    void drawImage(QPainter &painter);
    void drawSmoothImage(QPainter &painter, QRect &drawRect);

    QSize drawSize;
    QBrush backgroundBrush;
    QPixmap* pixmap;
    QPixmap cachedPixmap;
    QPoint translate;
    bool mouseDown;
    QPoint mouseDownPosition;
    QList<float> zoomLevels;
    int zoomLevel;
    int lastCachedWidth;
    bool fit;
    bool loaded;

};

#endif // VIEWWIDGET_H
