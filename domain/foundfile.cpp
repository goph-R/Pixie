#include "domain/foundfile.h"

FoundFile::FoundFile() {
    folder = false;
    image = false;
    name = "";
    folderPath = "";
    extension = "";
    width = 0;
    height = 0;
}

FoundFile::FoundFile(const FoundFile &other) {
    name = other.name;
    folder = other.folder;
    image = other.image;
    extension = other.extension;
    folderPath = other.folderPath;
    width = other.width;
    height = other.height;
}

FoundFile::~FoundFile() {
}

QString FoundFile::getName() {
    return name;
}

bool FoundFile::isFolder() {
    return folder;
}

QString FoundFile::getExtension() {
    return extension;
}

QString FoundFile::getFolderPath() {
    return folderPath;
}

bool FoundFile::isImage() {
    return image;
}

int FoundFile::getWidth() {
    return width;
}

int FoundFile::getHeight() {
    return height;
}
