QT       += core gui
QT += charts sql network printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressources.qrc

DISTFILES += \
    light.qss

# Page Comptes
INCLUDEPATH += $$PWD/ui/accounts
SOURCES += ui/accounts/accountcard.cpp ui/accounts/accountspage.cpp ui/accounts/accountdialog.cpp
HEADERS += ui/accounts/accountcard.h ui/accounts/accoutspage.h/ ui/accounts/accountdialog.h ui/accounts/AccountData.h ui/accounts/money.h
FORMS += ui/accounts/accountcard.ui ui/accounts/accountspage.ui
