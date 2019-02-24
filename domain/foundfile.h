#ifndef FOUNDFILE_H
#define FOUNDFILE_H

#include <QMetaType>
#include <QString>

class FoundFile
{
public:
    FoundFile();
    FoundFile(const FoundFile &other);
    virtual ~FoundFile();
    QString getName();
    QString getExtension();
    QString getFolderPath();
    bool isFolder();
    bool isImage();
    int getWidth();
    int getHeight();

private:
    QString name;
    QString extension;
    QString folderPath;
    bool folder;
    bool image;
    int width;
    int height;

    friend class FileManagerWorker;
};

Q_DECLARE_METATYPE(FoundFile);

#endif // FOUNDFILE_H
