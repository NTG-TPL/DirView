#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QFileSystemModel>
#include <QMap>
#include <QTimer>
#include <functional>
#include <set>

QString formatFileSize(qint64 size);

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    FileSystemModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool addWatcherPath(const QString& path);
private slots:
    void scheduleUpdate(const QString &path);
    void onDirectoryChanged();

private:
    QStringList getParentDirectories(const QString &dir);

    struct LengthComparator {
        bool operator()(const QString &lhs, const QString &rhs) const {
            return lhs.length() > rhs.length();
        }
    };

    qint64 calculateDirectorySize(const QDir &dir) const;

    QFileSystemWatcher *watcher;
    mutable QMap<QString, qint64> directorySizes;
    std::set<QString, LengthComparator> pendingUpdates;
    QTimer updateTimer;
};
