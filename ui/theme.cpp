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
    QString postfix = "themes/" + config->getTheme() + "/";
    QString themePath = config->getUserFolder() + postfix;
    QFileInfo info(themePath);
    if (info.exists(themePath)) {
        return themePath;
    }
    themePath = config->getAppFolder() + postfix;
    return themePath;
}

QString Theme::getPath(QString path) {
    QString result = path.replace(":/", getBasePath());
    QFileInfo info(result);
    if (info.exists()) {
        return result;
    }
    return path;
}

QPixmap Theme::getPixmap(QString path) {
    QPixmap pixmap(getPath(path));
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
