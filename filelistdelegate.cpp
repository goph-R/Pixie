#include "filelistdelegate.h"

#include <QPainter>
#include "filelistwidget.h"
#include <QDebug>
#include "filelistitem.h"

FileListDelegate::FileListDelegate(QWidget* parent) : QStyledItemDelegate (parent) {
    fileListWidget = static_cast<FileListWidget*>(parent);
    backgroundBrush = QBrush(QColor(242, 242, 242));
    textPen = QPen(QColor(24, 24, 24));
    borderBrush = QBrush(QColor(255, 255, 255));
}

FileListDelegate::~FileListDelegate() {
}

void FileListDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto path = index.data(Qt::UserRole).toString();
    FileListItem* item = fileListWidget->getItem(path);
    File* file = item->getFile();
    auto bgRect = option.rect;
    bgRect.adjust(0, 0, -4, -37);
    auto textRect = QRect(bgRect.x() + 6, bgRect.bottom(), bgRect.width() - 12, 28);

    // draw background
    p->setPen(Qt::NoPen);
    p->setBrush(backgroundBrush);
    p->drawRect(bgRect);

    // draw pixmap
    if (file->isFolder()) {
        drawPixmap(p, fileListWidget->folderPixmap, bgRect, false);
        bgRect.adjust(0, 8, 0, 0);
    }
    drawPixmap(p, item->pixmap, bgRect, file->isFolder() && !item->pixmap.isNull());

    // draw text
    p->setPen(textPen);
    p->drawText(textRect, Qt::AlignCenter, index.data().toString());
}

void FileListDelegate::drawPixmap(QPainter *p, QPixmap &pixmap, QRect &bgRect, bool border) const {
    auto rect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pixmap.size(), bgRect);
    if (border) {
        auto borderRect = rect;
        borderRect.adjust(-2, -2, 2, 2);
        p->setBrush(borderBrush);
        p->drawRect(borderRect);
    }
    p->drawPixmap(rect.x(), rect.y(), pixmap);
}
