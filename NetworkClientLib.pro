QT = core network

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SOURCES += \
    Network/AbstractNetworkRequest.cpp \
    Network/AbstractNetworkTask.cpp \
    Network/AbstractNetworkTaskManager.cpp \
    Network/DataProgress.cpp \
    Network/Http/HttpRequest.cpp \
    Network/Http/HttpRequestManager.cpp \
    Network/Http/HttpRequestTask.cpp \
    Task/AbstractTask.cpp \
    Task/TaskQueue.cpp \
    Task/TaskStorage.cpp \
    Test/Test.cpp \
    Test/TestClient.cpp \
    main.cpp

HEADERS += \
    Network/AbstractNetworkRequest.h \
    Network/AbstractNetworkTask.h \
    Network/AbstractNetworkTaskManager.h \
    Network/DataProgress.h \
    Network/Http/HttpRequest.h \
    Network/Http/HttpRequestManager.h \
    Network/Http/HttpRequestTask.h \
    Network/NetworkDefines.h \
    Network/NetworkTaskResult.h \
    Task/AbstractTask.h \
    Task/TaskDefines.h \
    Task/TaskQueue.h \
    Task/TaskStorage.h \
    Test/Test.h \
    Test/TestClient.h
