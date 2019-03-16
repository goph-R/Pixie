#ifndef THEME_H
#define THEME_H

#include <QWidget>
#include "domain/config.h"


class Theme
{
public:
    Theme(Config* config);
    void apply(QWidget* widget);
    QColor getColor(QString name, QColor defaultValue);
    QString getPath(QString path);
    QPixmap getPixmap(QString path);

private:
    QString getBasePath();
    void loadStyle();
    void loadIni();
    Config* config;
    QString style;
    QHash<QString, QVariant> values;
};

#endif // THEME_H
