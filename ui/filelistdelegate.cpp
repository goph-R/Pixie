#include "ui/filelistdelegate.h"

#include <QPainter>
#include "domain/file.h"
#include "ui/theme.h"
#include "ui/filelistwidget.h"
#include "ui/filelistitem.h"

#include <QDebug>

FileListDelegate::FileListDelegate(Theme* theme, QWidget* parent) : QStyledItemDelegate (parent) {
    fileListWidget = static_cast<FileListWidget*>(parent);
    backgroundBrush = theme->getColor("files.background_color", QColor(242, 242, 242));
    textPen = theme->getColor("files.text_color", QColor(24, 24, 24));
    borderBrush = theme->getColor("files.border_color", QColor(255, 255, 255));
    selectionBrush = theme->getColor("files.selection_color", QColor(0, 128, 255, 48));
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
    p->setPen(Qt::NoPen);

    // draw icon
    if (file->isFolder()) {
        if (file->isDotDot()) {
            drawPixmap(p, fileListWidget->backPixmap, bgRect, false);
        } else {
            drawPixmap(p, fileListWidget->folderPixmap, bgRect, false);
        }
    } else if (file->isImage()) {
        p->setBrush(backgroundBrush);
        p->drawRect(bgRect);
    }
    drawPixmap(p, item->pixmap, bgRect, file->isFolder() && !item->pixmap.isNull());

    // draw selection
    if (item->isSelected()) {
        auto selRect = option.rect;
        selRect.adjust(0, 0, -4, -4);
        p->setBrush(selectionBrush);
        p->drawRect(selRect);
    }

    // draw text
    QString text = index.data().toString();
    auto font = p->font();
    QFontMetrics fontMetrics(font);
    auto elidedText = fontMetrics.elidedText(text, Qt::ElideMiddle, textRect.width());
    p->setPen(textPen);
    p->drawText(textRect, Qt::AlignCenter, elidedText);
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
