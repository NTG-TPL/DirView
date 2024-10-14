#include <QScreen>
#include <QScroller>
#include <QDebug>

#include <chrono>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    using namespace std::chrono_literals;

    fileSystemModel = new FileSystemModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(fileSystemModel);
    proxyModel->setFilterKeyColumn(0);
    proxyModel->setRecursiveFilteringEnabled(true);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    treeView = new QTreeView(this);
    treeView->setModel(proxyModel);
    treeView->setAnimated(false);
    int offset = 20;
    treeView->setIndentation(offset);
    treeView->setSortingEnabled(true);
    const QSize availableSize = treeView->screen()->availableGeometry().size();
    treeView->resize(availableSize/2);
    treeView->setColumnWidth(0, treeView->width()/2);
    setTreeHomeRoot(*treeView, QDir::homePath());

    QScroller::grabGesture(treeView, QScroller::TouchGesture);

    filterLineEdit = new QLineEdit(this);
    filterLineEdit->setEnabled(false);
    filterLineEdit->setPlaceholderText("Пожалуйста, подождите: файловая модель загружается ... ");

    filterTimer = new QTimer(this);
    filterTimer->setSingleShot(true);
    std::chrono::milliseconds delay = 300ms;
    filterTimer->setInterval(delay);

    connect(fileSystemModel, &FileSystemModel::directoryLoaded, this, &MainWindow::onDirectoryLoaded);
    connect(filterLineEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);
    connect(filterTimer, &QTimer::timeout, this, &MainWindow::applyFilter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(filterLineEdit);
    layout->addWidget(treeView);

    setWindowTitle(tr("Dir View"));
    resize(treeView->size());
}

void MainWindow::onDirectoryLoaded(const QString &path)
{
    if (path == QDir::homePath()) {
        QModelIndex homeIndex = fileSystemModel->index(QDir::cleanPath(QDir::homePath()));
        loadAllNodes(homeIndex);
        filterLineEdit->setPlaceholderText("Введите название файла/папки ...");
        filterLineEdit->setEnabled(true);
    }
}

void MainWindow::onFilterTextChanged(const QString &)
{
    filterTimer->start();
}

void MainWindow::setTreeHomeRoot(QTreeView& tree, const QString& path)
{
    QModelIndex homeIndex = fileSystemModel->index(QDir::cleanPath(path));
    if (homeIndex.isValid()) {
        tree.setRootIndex(proxyModel->mapFromSource(homeIndex));
    }
}

void MainWindow::applyFilter()
{
    proxyModel->setFilterWildcard(filterLineEdit->text());
    setTreeHomeRoot(*treeView, QDir::homePath());
}

void MainWindow::loadAllNodes(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int rowCount = fileSystemModel->rowCount(index);
    for (int i = 0; i < rowCount; ++i) {
        QModelIndex childIndex = fileSystemModel->index(i, 0, index);
        if (fileSystemModel->canFetchMore(childIndex)) {
            fileSystemModel->fetchMore(childIndex);
        }
        loadAllNodes(childIndex);
    }
}
