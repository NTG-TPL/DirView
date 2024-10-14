QT += widgets
requires(qtConfig(treeview))

SOURCES       = main.cpp

# install
target.path = .
INSTALLS += target
