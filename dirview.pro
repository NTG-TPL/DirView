QT += widgets testlib
requires(qtConfig(treeview))

SOURCES       = main.cpp \
    filesystemmodel.cpp \
    mainwindow.cpp

HEADERS += \
    filesystemmodel.h \
    mainwindow.h

# install
target.path = .
INSTALLS += target
