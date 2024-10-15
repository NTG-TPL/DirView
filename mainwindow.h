#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QTreeView>
#include <QVBoxLayout>
#include <QTimer>
#include <QSortFilterProxyModel>
#include "filesystemmodel.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onDirectoryLoaded(const QString &path);
    void onFilterTextChanged(const QString &text);
    void setTreeHomeRoot(QTreeView& tree, const QString& path);
    void applyFilter();

private:
    void loadAllNodes(const QModelIndex &index);

    FileSystemModel *fileSystemModel;
    QSortFilterProxyModel *proxyModel;
    QTreeView *treeView;
    QLineEdit *filterLineEdit;
    QTimer *filterTimer;
};

#endif // MAINWINDOW_H
