#include "viewwidget.h"

#include <QPainter>
#include <QStyle>

ViewWidget::ViewWidget(QWidget* parent) : QWidget(parent) {
    backgroundBrush = QBrush(QColor(24, 24, 24));
}

void ViewWidget::setImage(QString path) {
    pixmap.load(path);
    update();
}

void ViewWidget::paintEvent(QPaintEvent *event __attribute__((unused))) {
    QPainter p(this);

    // draw background
    p.setPen(Qt::NoPen);
    p.setBrush(backgroundBrush);
    p.drawRect(rect());

    QSize s;
    float pw = pixmap.width();
    float ph = pixmap.height();
    float vw = width();
    float vh = height();

    // if bigger than the view calculate the size
    if (pw > vw || ph > vh) {
        if (pw / ph > vw / vh) {
            s.setWidth(static_cast<int>(vw));
            s.setHeight(static_cast<int>((vw / pw) * ph));
        } else {
            s.setWidth(static_cast<int>((vh / ph) * pw));
            s.setHeight(static_cast<int>(vh));
        }
    } else {
        s = pixmap.size();
    }

    // draw image on center
    auto r = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, s, rect());
    p.drawPixmap(r, pixmap);
}


