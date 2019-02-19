#include "filelistdelegate.h"

#include <QPainter>
#include "filelistwidget.h"
#include <QDebug>
#include "filelistitem.h"

FileListDelegate::FileListDelegate(QWidget* parent) : QStyledItemDelegate (parent) {
    fileListWidget = static_cast<FileListWidget*>(parent);
    brush = QBrush(QColor(242, 242, 242));
    pen = QPen(QColor(24, 24, 24));
}

FileListDelegate::~FileListDelegate() {
}

void FileListDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto path = index.data(Qt::UserRole).toString();
    FileListItem* item = fileListWidget->getItem(path);

    auto bgRect = option.rect;
    bgRect.adjust(0, 0, -4, -37);
    p->setBrush(brush);
    p->setPen(Qt::NoPen);
    p->drawRect(bgRect);

    auto rect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, item->pixmap.size(), bgRect);
    p->drawPixmap(rect.x(), rect.y(), item->pixmap);

    auto textRect = QRect(bgRect.x(), bgRect.bottom() + 2, bgRect.width(), 32);
    p->drawRect(textRect);
    p->setPen(pen);
    p->drawText(textRect, Qt::AlignCenter, index.data().toString());
}
