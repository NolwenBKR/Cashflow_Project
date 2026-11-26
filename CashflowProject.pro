QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/core/services/DataNotifier.cpp \
    src/core/services/ImportExportService.cpp \
    src/core/models/Budget.cpp \
    src/core/models/Category.cpp \
    src/core/models/Compte.cpp \
    src/core/models/Transaction.cpp \
    src/database/DatabaseManager.cpp \
    src/database/repositories/AccountRepository.cpp \
    src/database/repositories/BudgetRepository.cpp \
    src/database/repositories/CategoryRepository.cpp \
    src/database/repositories/TransactionRepository.cpp \
    src/main.cpp \
    src/ui/dialogs/CompteDialog.cpp \
    src/ui/dialogs/ResetDialog.cpp \
    src/ui/utils/Formatage.cpp \
    src/ui/views/Comptes/ComptesPage.cpp \
    src/ui/views/ImportExport/ImportExportPage.cpp \
    src/ui/views/MainWindow/mainwindow.cpp

HEADERS += \
    src/core/services/DataNotifier.h \
    src/core/services/ImportExportService.h \
    src/core/interfaces/IAccountRepository.h \
    src/core/interfaces/IBudgetRepository.h \
    src/core/interfaces/ICategoryRepository.h \
    src/core/interfaces/ITransactionRepository.h \
    src/core/models/BaseModel.h \
    src/core/models/Budget.h \
    src/core/models/Category.h \
    src/core/models/Compte.h \
    src/core/models/Transaction.h \
    src/database/DatabaseManager.h \
    src/database/repositories/AccountRepository.h \
    src/database/repositories/BudgetRepository.h \
    src/database/repositories/CategoryRepository.h \
    src/database/repositories/TransactionRepository.h \
    src/ui/dialogs/CompteDialog.h \
    src/ui/dialogs/ResetDialog.h \
    src/ui/utils/Formatage.h \
    src/ui/views/Comptes/ComptesPage.h \
    src/ui/views/ImportExport/ImportExportPage.h \
    src/ui/views/MainWindow/mainwindow.h

FORMS += \
    src/ui/views/MainWindow/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
