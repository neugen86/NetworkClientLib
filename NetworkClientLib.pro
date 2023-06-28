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
    Network/DataProgress.cpp \
    Network/Http/HttpRequest.cpp \
    Network/Http/HttpRequestManager.cpp \
    Network/Http/HttpRequestTask.cpp \
    Network/NetworkRequest.cpp \
    Network/NetworkTask.cpp \
    Network/NetworkTaskManager.cpp \
    Task/SharedTaskStorage.cpp \
    Task/Task.cpp \
    Task/TaskQueue.cpp \
    Test/Test.cpp \
    Test/TestClient.cpp \
    main.cpp

HEADERS += \
    Network/DataProgress.h \
    Network/Http/HttpRequest.h \
    Network/Http/HttpRequestManager.h \
    Network/Http/HttpRequestTask.h \
    Network/NetworkDefines.h \
    Network/NetworkRequest.h \
    Network/NetworkTask.h \
    Network/NetworkTaskManager.h \
    Network/NetworkTaskResult.h \
    Task/SharedTaskStorage.h \
    Task/Task.h \
    Task/TaskDefines.h \
    Task/TaskQueue.h \
    Test/Test.h \
    Test/TestClient.h
