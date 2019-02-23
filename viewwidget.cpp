#include "viewwidget.h"

#include <QPainter>
#include <QStyle>
#include <QShortcut>

ViewWidget::ViewWidget(QWidget* parent) : QWidget(parent) {
    backgroundBrush = QBrush(QColor(24, 24, 24));
    fit = true;
    mouseDown = false;
    setMouseTracking(true);
    new QShortcut(QKeySequence(Qt::Key_Up), this, SLOT(translateUp()));
    new QShortcut(QKeySequence(Qt::Key_Down), this, SLOT(translateDown()));
    new QShortcut(QKeySequence(Qt::Key_Plus), this, SLOT(zoomIn()));
    new QShortcut(QKeySequence(Qt::Key_Minus), this, SLOT(zoomOut()));

    zoomLevel = zoomLevel100;
    zoomLevels << 0.01f << 0.02f << 0.05f << 0.075f << 0.10f << 0.15f << 0.20f;
    zoomLevels << 0.25f << 0.30f << 0.40f << 0.50f << 0.66f << 0.75f << 1.00f;
    zoomLevels << 1.33f << 1.66f << 2.00f << 3.00f << 5.00f << 10.0f << 20.0f;
}

void ViewWidget::reset() {
    translate.setX(0);
    translate.setY(0);
    mouseDown = false;
}

void ViewWidget::setImage(const QImage image) {
    this->image = image;
    update();
}

bool ViewWidget::isFit() {
    return fit;
}

void ViewWidget::setFit(bool value) {
    fit = value;
    if (fit) {
        translate.setX(0);
        translate.setY(0);
        zoomLevel = zoomLevel100;
    }
    update();
}

void ViewWidget::mousePressEvent(QMouseEvent* event) {
    mouseDown = true;
    if (!fit) {
        mouseDownPosition = event->pos() - translate;
    }
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!fit && mouseDown) {
        translate = event->pos() - mouseDownPosition;
        update();
    }
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event __attribute__((unused))) {
    mouseDown = false;
    update();
}

void ViewWidget::mouseDoubleClickEvent(QMouseEvent* event __attribute__((unused))) {
    emit doubleClickedSignal();
}

void ViewWidget::translateDown() {
    translate.setY(translate.y() - 40);
    update();
}

void ViewWidget::translateUp() {
    translate.setY(translate.y() + 40);
    update();
}

void  ViewWidget::zoomIn() {
    zoomLevel++;
    if (zoomLevel >= zoomLevels.size()) {
        zoomLevel = zoomLevels.size() - 1;
    }
    update();
}

void ViewWidget::zoomOut() {
    zoomLevel--;
    if (zoomLevel < 0) {
        zoomLevel = 0;
    }
    update();
}

void ViewWidget::paintEvent(QPaintEvent *event __attribute__((unused))) {
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(backgroundBrush);
    p.drawRect(rect());

    if (image.isNull()) {
        return;
    }

    float pw = image.width();
    float ph = image.height();
    float vw = width();
    float vh = height();
    bool resized = zoomLevel < zoomLevel100;
    QSize drawSize = image.size();

    float zoom = zoomLevels.at(zoomLevel);
    float drawWidth = drawSize.width() * zoom;
    float drawHeight = drawSize.height() * zoom;
    drawSize.setWidth(static_cast<int>(drawWidth));
    drawSize.setHeight(static_cast<int>(drawHeight));

    if (fit && (pw > vw || ph > vh)) {
        if (pw / ph > vw / vh) {
            drawSize.setWidth(static_cast<int>(vw));
            drawSize.setHeight(static_cast<int>((vw / pw) * ph));
        } else {
            drawSize.setWidth(static_cast<int>((vh / ph) * pw));
            drawSize.setHeight(static_cast<int>(vh));
        }
        resized = true;
    }

    QRect drawRect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, drawSize, rect());

    drawRect.setTopLeft(drawRect.topLeft() + translate);
    drawRect.setBottomRight(drawRect.bottomRight() + translate);

    if (resized) {
        auto smoothPixmap = image.scaled(drawSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        p.drawImage(drawRect, smoothPixmap);
    } else {
        p.drawImage(drawRect, image);
    }
}


