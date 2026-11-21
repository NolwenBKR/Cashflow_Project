include("/home/ednabkr/Documents/Cashflow/build/Desktop-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Cashflow-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/ednabkr/Documents/Cashflow/build/Desktop-Debug/Cashflow"
    GENERATE_QT_CONF
)
