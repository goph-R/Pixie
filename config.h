#ifndef CONFIG_H
#define CONFIG_H

#include <QStringList>

class Config
{
public:
    Config();
    const QStringList getImageExtensions();
    const QStringList getImageFileNameFilters();
private:
    QStringList imageExtensions;
    QStringList imageFileNameFilters;
};

#endif // CONFIG_H
