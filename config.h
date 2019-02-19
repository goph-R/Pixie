#ifndef CONFIG_H
#define CONFIG_H

#include <QStringList>

class Config
{
public:
    Config();
    const QStringList getImageExtensions();
private:
    QStringList imageExtensions;
};

#endif // CONFIG_H
