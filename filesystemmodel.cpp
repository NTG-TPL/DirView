#include "filesystemmodel.h"
#include <QStringList>

#include <chrono>

FileSystemModel::FileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{
    using namespace std::chrono_literals;

    setRootPath(QDir::homePath());
    setFilter(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);
    setOption(QFileSystemModel::DontUseCustomDirectoryIcons);

    watcher = new QFileSystemWatcher(this);

    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &FileSystemModel::scheduleUpdate);
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &FileSystemModel::scheduleUpdate);
    connect(&updateTimer, &QTimer::timeout, this, &FileSystemModel::onDirectoryChanged);
    updateTimer.setSingleShot(true);
    auto delay = 10ms;
    updateTimer.setInterval(delay);
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (index.column() == 1 && role == Qt::DisplayRole) {
        if (isDir(index)) {
            QString path = filePath(index);
            qint64 size = calculateDirectorySize(path);
            directorySizes[path] = size;
            return formatFileSize(size);
        }
    }
    return QFileSystemModel::data(index, role);
}

bool FileSystemModel::addWatcherPath(const QString& path){
    return watcher->addPath(path);
}

qint64 FileSystemModel::calculateDirectorySize(const QDir& dir) const
{
    qint64 size = 0;
    QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);
    for (const QFileInfo &fileInfo : list) {
        if (fileInfo.isDir()) {
            auto childPath = fileInfo.absoluteFilePath();
            qint64 dirSize = 0;
            if(directorySizes.contains(childPath)){
                dirSize = directorySizes.value(childPath);
            }else {
                dirSize = calculateDirectorySize(childPath);
                directorySizes[childPath] = dirSize;
            }
            size += dirSize;
        } else {
            size += fileInfo.size();
        }
    }
    return size;
}

void FileSystemModel::scheduleUpdate(const QString &path) {
    for (const QString &item : getParentDirectories(path)) {
        pendingUpdates.insert(item);
    }
    updateTimer.start();
}

void FileSystemModel::onDirectoryChanged()
{
    for (const QString &path : pendingUpdates) {
        directorySizes.remove(path);
        if (QDir(path).exists()) {
            directorySizes[path] = calculateDirectorySize(path);
            watcher->addPath(path);
        } else {
            watcher->removePath(path);
        }
    }
    pendingUpdates.clear();
}

QString formatFileSize(qint64 size) {
    static qint64   kb{1024},
                    mb{1024 * 1024},
                    gb{1024 * 1024 * 1024};

    static auto format = [](double size){
        return QString::number(size, 'f', 2);
    };

    static auto getSize = [](qint64 size, qint64 measure){
        return double(size / measure) + double(size % measure)/double(measure);
    };

    if (size < kb) {
        return QString("%1 B").arg(format(size));
    } else if (size < mb) {
        return QString("%1 KB").arg(format(getSize(size, kb)));
    } else if (size < gb) {
        return QString("%1 MB").arg(format(getSize(size, mb)));
    } else {
        return QString("%1 GB").arg(format(getSize(size, gb)));
    }
}

QStringList FileSystemModel::getParentDirectories(const QString &path) {
    QStringList parentDirectories;
    QDir currentDir(path);

    parentDirectories.append(currentDir.absolutePath());

    while (currentDir.cdUp()) {
        if(currentDir == QDir::home()){
            break;
        }
        parentDirectories.append(currentDir.absolutePath());
    }

    return parentDirectories;
}
