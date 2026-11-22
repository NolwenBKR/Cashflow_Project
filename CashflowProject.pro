QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/core/models/Budget.cpp \
    src/core/models/Category.cpp \
    src/core/models/Compte.cpp \
    src/core/models/Transaction.cpp \
    src/database/DatabaseManager.cpp \
    src/main.cpp \
    src/ui/views/MainWindow/mainwindow.cpp

HEADERS += \
    src/core/models/Budget.h \
    src/core/models/Category.h \
    src/core/models/Compte.h \
    src/core/models/Transaction.h \
    src/database/DatabaseManager.h \
    src/ui/views/MainWindow/mainwindow.h

FORMS += \
    src/ui/views/MainWindow/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
