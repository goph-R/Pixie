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
    QBrush brush;
    QPen pen;
};

#endif // FILELISTDELEGATE_H
