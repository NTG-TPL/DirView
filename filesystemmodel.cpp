#include "filesystemmodel.h"

FileSystemModel::FileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{
    setRootPath(QDir::homePath());
    setFilter(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);
    setOption(QFileSystemModel::DontUseCustomDirectoryIcons);
}
