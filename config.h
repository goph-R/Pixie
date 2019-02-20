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


private:
    QStringList imageExtensions;
    QStringList imageFileNameFilters;
    int thumbnailSize;
};

#endif // CONFIG_H
