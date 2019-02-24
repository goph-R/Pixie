#ifndef FILELISTDELEGATE_H
#define FILELISTDELEGATE_H

#include <QStyledItemDelegate>
#include <QPen>

class FileListWidget;

class FileListDelegate : public QStyledItemDelegate
{
public:
    FileListDelegate(QWidget *parent);
    virtual ~FileListDelegate() override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    FileListWidget* fileListWidget;
    QBrush backgroundBrush;
    QBrush borderBrush;
    QBrush selectionBrush;
    QPen textPen;
    void drawPixmap(QPainter *p, QPixmap &pixmap, QRect &rect, bool border) const;
};

#endif // FILELISTDELEGATE_H
