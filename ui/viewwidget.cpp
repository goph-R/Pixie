#include "viewwidget.h"

#include <QPainter>
#include <QStyle>
#include <QShortcut>
#include <QMouseEvent>

#include <QDebug>

ViewWidget::ViewWidget(QWidget* parent) : QWidget(parent) {
    backgroundBrush = QBrush(QColor(24, 24, 24));
    fit = true;
    mouseDown = false;
    zoomLevel = ZOOM_ORIGINAL;
    lastCachedWidth = 0;
    zoomLevels << 0.05f << 0.07f << 0.10f << 0.15f << 0.20f << 0.25f;
    zoomLevels << 0.30f << 0.40f << 0.50f << 0.62f << 0.75f << 1.00f;
    zoomLevels << 1.33f << 1.66f << 2.00f << 3.00f << 5.00f << 10.0f;
    lastCachedWidth = 0;
    pixmap = nullptr;
    setMouseTracking(true);
}

QPixmap* ViewWidget::getPixmap() {
    return pixmap;
}

void ViewWidget::setLoaded() {
    loaded = true;
    lastCachedWidth = 0;
    update();
}

void ViewWidget::setPixmap(QPixmap* p, bool u) {
    loaded = false;
    if (pixmap != nullptr) {
        delete pixmap;
    }
    pixmap = p;
    if (pixmap != nullptr && u) {
        update();
    }
}

bool ViewWidget::isFit() {
    return fit;
}

void ViewWidget::setFit(bool value) {
    fit = value;
    if (fit) {
        translate.setX(0);
        translate.setY(0);
        zoomLevel = ZOOM_ORIGINAL;
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
        return;
    }
    translate.setY(translate.y() - 40);
    update();
}

void ViewWidget::translateUp() {
    if (!fit) {
        return;
    }
    translate.setY(translate.y() + 40);
    update();
}

void  ViewWidget::zoomIn() {
    if (fit) {
        return;
    }
    zoomLevel++;
    if (zoomLevel >= zoomLevels.size()) {
        zoomLevel = zoomLevels.size() - 1;
    }
    update();
}

void ViewWidget::zoomOut() {
    if (fit) {
        return;
    }
    zoomLevel--;
    if (zoomLevel < 0) {
        zoomLevel = 0;
    }
    update();
}

void ViewWidget::paintEvent(QPaintEvent *event __attribute__((unused))) {
    calculateDrawSize();
    limitTranslate();
    QPainter p(this);
    drawBackground(p);
    drawImage(p);
}


void ViewWidget::calculateDrawSize() {
    return fit ? calculateFittedDrawSize() : calculateZoomedDrawSize();
}

void ViewWidget::calculateFittedDrawSize() {
    if (pixmap == nullptr) {
        return;
    }
    drawSize = pixmap->size();
    float pw = drawSize.width();
    float ph = drawSize.height();
    float vw = width();
    float vh = height();
    if (pw <= vw && ph <= vh) {
        return;
    }
    if (pw / ph > vw / vh) {
        drawSize.setWidth(static_cast<int>(vw));
        drawSize.setHeight(static_cast<int>((vw / pw) * ph));
    } else {
        drawSize.setWidth(static_cast<int>((vh / ph) * pw));
        drawSize.setHeight(static_cast<int>(vh));
    }
}

void ViewWidget::calculateZoomedDrawSize() {
    drawSize = pixmap->size();
    float zoom = zoomLevels.at(zoomLevel);
    float drawWidth = drawSize.width() * zoom;
    float drawHeight = drawSize.height() * zoom;
    drawSize.setWidth(static_cast<int>(drawWidth));
    drawSize.setHeight(static_cast<int>(drawHeight));
}

void ViewWidget::limitTranslate() {
    auto limitX = (drawSize.width() - width()) / 2;
    auto limitY = (drawSize.height() - height()) / 2;
    if (drawSize.width() < width()) {
        translate.setX(0);
    } else if (translate.x() < -limitX) {
        translate.setX(-limitX);
    } else if (translate.x() > limitX) {
        translate.setX(limitX);
    }
    if (drawSize.height() < height()) {
        translate.setY(0);
    } else if (translate.y() < -limitY) {
        translate.setY(-limitY);
    } else if (translate.y() > limitY) {
        translate.setY(limitY);
    }
}

void ViewWidget::drawBackground(QPainter &p) {
    p.setPen(Qt::NoPen);
    p.setBrush(backgroundBrush);
    p.drawRect(rect());
}

void ViewWidget::drawImage(QPainter &p) {
    if (pixmap == nullptr) {
        return;
    }
    QRect drawRect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, drawSize, rect());
    if (!fit) {
        drawRect.setTopLeft(drawRect.topLeft() + translate);
        drawRect.setBottomRight(drawRect.bottomRight() + translate);
    }
    if (drawSize.width() < pixmap->width() || drawSize.height() < pixmap->height()) {
        drawSmoothImage(p, drawRect);
    } else {
        p.drawPixmap(drawRect, *pixmap);
    }
}

void ViewWidget::drawSmoothImage(QPainter &p, QRect &drawRect) {
    float iw = pixmap->width();
    float ih = pixmap->height();
    float rw = drawRect.width();
    float rh = drawRect.height();
    if (loaded && (rw / iw < 1.0f || rh / ih < 1.0f)) {
        if (lastCachedWidth != drawRect.width()) {
            lastCachedWidth = drawRect.width();
            cachedPixmap = pixmap->scaled(drawRect.width(), drawRect.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
        p.drawPixmap(drawRect, cachedPixmap);
    } else {
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.drawPixmap(drawRect, *pixmap);
    }
}




