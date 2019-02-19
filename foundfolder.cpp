#include "foundfolder.h"

FoundFolder::FoundFolder() {
}

FoundFolder::FoundFolder(const FoundFolder &other) {
    name = other.name;
    folderPath = other.folderPath;
}

FoundFolder::~FoundFolder() {
}

QString FoundFolder::getName() {
    return name;
}

QString FoundFolder::getFolderPath() {
    return folderPath;
}
