#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>

class ViewWidget : public QWidget
{
public:
    ViewWidget(QWidget* parent=nullptr);
    void setImage(QString path);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QBrush backgroundBrush;
    QPixmap pixmap;
};

#endif // VIEWWIDGET_H
