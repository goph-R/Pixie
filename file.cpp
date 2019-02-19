#include "file.h"

#include <QDebug>

File::File(File* parent, QString name, bool folder) {
    this->name = name;
    this->parent = parent;
    this->folder = folder;
    this->extension = "";
    QString parentPath = "";
    QString endSlash = "";
    if (parent != nullptr) {
        parentPath = parent->getPath();
        endSlash = folder && parentPath != "" ? "/" : "";
    }
    path = parentPath + name + endSlash;
}

File::~File() {
    removeChildren();
}

QString File::getPath() {
    return path;
}

QString File::getName() {
    return name;
}

File* File::getParent() {
    return parent;
}

void File::addChild(File* file) {
    children.append(file);
}

QString File::getExtension() {
    return extension;
}

const QList<File*> File::getChildren() {
    return children;
}

void File::removeChildren() {
    while (children.size()) {
        File* file = children.takeLast();
        delete file;
    }
}

bool File::isFolder() {
    return folder;
}
