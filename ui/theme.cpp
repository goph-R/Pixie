#include "theme.h"

#include <QFile>
#include <QFileInfo>
#include <QSettings>

#include <QDebug>

Theme::Theme(Config* config) {
    this->config = config;
    loadStyle();
    loadIni();
}

void Theme::apply(QWidget* widget) {
    widget->setStyleSheet(style);
}

QString Theme::getBasePath() {
    return config->getUserFolder() + "themes/" + config->getTheme() + "/";
}

QString Theme::getPath(QString path) {
    return path.replace(":/", getBasePath());
}

QPixmap Theme::getPixmap(QString path) {
    QString pixmapPath = path;
    QString themePath = getPath(path);
    QFileInfo info(themePath);
    if (info.exists(themePath)) {
        pixmapPath = themePath;
    }
    QPixmap pixmap(pixmapPath);
    return pixmap;
}

QColor Theme::getColor(QString name, QColor defaultValue) {
    QColor result(defaultValue);
    if (values.contains(name)) {
        result.setNamedColor(values.value(name).toString());
    }
    return result;
}

void Theme::loadStyle() {
    if (config->getTheme() == "default") {
        style = "";
        return;
    }
    QFile file(getPath(":/style.qss"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    auto ba = file.readAll();
    style = QString::fromUtf8(ba);
    style.replace("url(\":/", "url(\"" + getBasePath());
    file.close();
}

void Theme::loadIni() {
    if (config->getTheme() == "default") {
        values.clear();
        return;
    }
    QSettings settings(getPath(":/style.ini"), QSettings::IniFormat);
    settings.sync();
    foreach (auto group, settings.childGroups()) {
        settings.beginGroup(group);
        foreach (auto key, settings.childKeys()) {
            values.insert(group + "." + key, settings.value(key));
        }
        settings.endGroup();
    }
}
