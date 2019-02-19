#include "foundfile.h"

FoundFile::FoundFile() {
    folder = false;
    name = "";
    folderPath = "";
    extension = "";
}

FoundFile::FoundFile(const FoundFile &other) {
    name = other.name;
    folder = other.folder;
    extension = other.extension;
    folderPath = other.folderPath;
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
