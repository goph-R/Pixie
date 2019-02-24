#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QListWidget>
#include <QHash>

class Config;
class File;
class FileListDelegate;
class FileListItem;

class FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    FileListWidget(Config* config);
    virtual ~FileListWidget() override;
    FileListItem* createItem(File* file);
    bool hasItem(QString path);
    FileListItem* getItem(QString path);
    void setErrorPixmap(QString path);
    virtual void clear();
    void select(QString path);
    virtual void keyPressEvent(QKeyEvent* event) override;

public slots:
    void backspacePressedSlot();

signals:
    void backspacePressed();
    void enterPressed();

private:
    void createPixmaps();
    QPixmap createPixmap(QString path);
    Config* config;
    QHash<QString, FileListItem*> itemsByPath;
    FileListDelegate* delegate;
    QPixmap folderPixmap;
    QPixmap filePixmap;
    QPixmap imagePixmap;
    QPixmap imageErrorPixmap;

    friend class FileListDelegate;
};

#endif // FILELISTWIDGET_H
