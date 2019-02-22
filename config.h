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
    const QString getCacheFolder();

private:
    void setUpImageExtensions();
    void setUpCacheFolder();
    QStringList imageExtensions;
    QStringList imageFileNameFilters;
    int thumbnailSize;
    QString cacheFolder;

};

#endif // CONFIG_H
