#include "viewwidget.h"

#include <QPainter>
#include <QStyle>

ViewWidget::ViewWidget(QWidget* parent) : QWidget(parent) {
    backgroundBrush = QBrush(QColor(24, 24, 24));
    fit = true;
    mouseDown = false;
    setMouseTracking(true);
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
    bool resized = false;
    QSize drawSize = image.size();

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

    if (!fit) {
        drawRect.setTopLeft(drawRect.topLeft() + translate);
        drawRect.setBottomRight(drawRect.bottomRight() + translate);
    }

    if (resized) {
        auto smoothPixmap = image.scaled(drawSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        p.drawImage(drawRect, smoothPixmap);
    } else {
        p.drawImage(drawRect, image);
    }
}


