#ifndef FOUNDFOLDER_H
#define FOUNDFOLDER_H

#include <QMetaType>
#include <QString>

class FoundFolder
{
public:
    FoundFolder();
    FoundFolder(const FoundFolder &other);
    virtual ~FoundFolder();
    QString getName();
    QString getFolderPath();

private:
    QString name;
    QString folderPath;

    friend class FileManagerWorker;
};

Q_DECLARE_METATYPE(FoundFolder);

#endif // FOUNDFOLDER_H
