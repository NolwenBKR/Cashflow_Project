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
    src/core/router/AppRouter.cpp \
    src/core/router/Route.cpp \
    src/database/DatabaseManager.cpp \
    src/database/repositories/AccountRepository.cpp \
    src/database/repositories/BudgetRepository.cpp \
    src/database/repositories/CategoryRepository.cpp \
    src/database/repositories/TransactionRepository.cpp \
    src/main.cpp \
    src/resources/routes/routes.cpp \
    src/ui/layouts/MainLayout.cpp \
    src/ui/layouts/Sidebar.cpp \
    src/ui/views/MainWindow/mainwindow.cpp

HEADERS += \
    src/core/interfaces/IAccountRepository.h \
    src/core/interfaces/IBudgetRepository.h \
    src/core/interfaces/ICategoryRepository.h \
    src/core/interfaces/ITransactionRepository.h \
    src/core/models/BaseModel.h \
    src/core/models/Budget.h \
    src/core/models/Category.h \
    src/core/models/Compte.h \
    src/core/models/Transaction.h \
    src/core/router/AppRouter.h \
    src/core/router/Route.h \
    src/database/DatabaseManager.h \
    src/database/repositories/AccountRepository.h \
    src/database/repositories/BudgetRepository.h \
    src/database/repositories/CategoryRepository.h \
    src/database/repositories/TransactionRepository.h \
    src/resources/routes/routes.h \
    src/ui/layouts/MainLayout.h \
    src/ui/layouts/Sidebar.h \
    src/ui/views/MainWindow/mainwindow.h

FORMS += \
    src/ui/views/MainWindow/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
