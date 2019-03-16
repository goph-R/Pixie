#ifndef CONFIG_H
#define CONFIG_H

#include <QStringList>

class Config
{
public:
    Config();
    const QStringList getImageExtensions();
    const QStringList getImageFileNameFilters();
    int getThumbnailSize();
    const QString getThumbnailsPath();
    bool useBackslash();
    int getMaxThreadCount();
    const QString getUserFolder();
    const QString getTheme();

private:
    static QString USER_FOLDER;
    void setUpImageExtensions();
    void setUpUserFolder();
    QStringList imageExtensions;
    QStringList imageFileNameFilters;
    int thumbnailSize;
    QString thumbnailsPath;
    QString cacheFolder;
    QString theme;

};

#endif // CONFIG_H
