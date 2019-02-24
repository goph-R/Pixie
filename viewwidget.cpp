#include "viewwidget.h"

#include <QPainter>
#include <QStyle>
#include <QShortcut>
#include <QDebug>

ViewWidget::ViewWidget(QWidget* parent) : QWidget(parent) {
    backgroundBrush = QBrush(QColor(24, 24, 24));
    fit = true;
    mouseDown = false;
    zoomLevel = zoomLevel100;
    zoomLevels << 0.01f << 0.02f << 0.05f << 0.075f << 0.10f << 0.15f << 0.20f;
    zoomLevels << 0.25f << 0.30f << 0.40f << 0.50f << 0.66f << 0.75f << 1.00f;
    zoomLevels << 1.33f << 1.66f << 2.00f << 3.00f << 5.00f << 10.0f << 20.0f;
    setMouseTracking(true);
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
    if (!fit) {
        translate.setY(translate.y() - 40);
        update();
    }
}

void ViewWidget::translateUp() {
    if (!fit) {
        translate.setY(translate.y() + 40);
        update();
    }
}

void  ViewWidget::zoomIn() {
    if (!fit) {
        zoomLevel++;
        if (zoomLevel >= zoomLevels.size()) {
            zoomLevel = zoomLevels.size() - 1;
        }
        update();
    }
}

void ViewWidget::zoomOut() {
    if (!fit) {
        zoomLevel--;
        if (zoomLevel < 0) {
            zoomLevel = 0;
        }
        update();
    }
}

QSize ViewWidget::getZoomedSize() {
    QSize result = image.size();
    float zoom = zoomLevels.at(zoomLevel);
    float drawWidth = result.width() * zoom;
    float drawHeight = result.height() * zoom;
    result.setWidth(static_cast<int>(drawWidth));
    result.setHeight(static_cast<int>(drawHeight));
    return result;
}

void ViewWidget::limitTranslate() {
    auto size = getZoomedSize();
    auto limitX = (size.width() - width()) / 2;
    auto limitY = (size.height() - height()) / 2;
    if (size.width() < width()) {
        translate.setX(0);
    } else if (translate.x() < -limitX) {
        translate.setX(-limitX);
    } else if (translate.x() > limitX) {
        translate.setX(limitX);
    }
    if (size.height() < height()) {
        translate.setY(0);
    } else if (translate.y() < -limitY) {
        translate.setY(-limitY);
    } else if (translate.y() > limitY) {
        translate.setY(limitY);
    }
}

void ViewWidget::paintEvent(QPaintEvent *event __attribute__((unused))) {

    limitTranslate();

    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(backgroundBrush);
    p.drawRect(rect());

    if (image.isNull()) {
        return;
    }

    auto drawSize = getZoomedSize();
    float pw = drawSize.width();
    float ph = drawSize.height();
    float vw = width();
    float vh = height();
    bool resized = zoomLevel < zoomLevel100;

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


