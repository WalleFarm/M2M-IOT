QT += quick charts 
QT += core  gui network websockets  multimedia
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += resources_big

#QMAKE_CXXFLAGS_RELEASE -= -O2 
#QMAKE_CXXFLAGS_RELEASE += -O0 

#QMAKE_CFLAGS_RELEASE -= -O2 
#QMAKE_CFLAGS_RELEASE += -O0 


# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/QZXing/QZXing.pri)

SOURCES += \
        MainInterface.cpp \
        center/AccountMan.cpp \
        center/CenterMan.cpp \
        center/DecodeThread.cpp \
        drivers/DrvCommon.cpp \
        drivers/encrypt/mbedtls/aes.c \
        main.cpp \
        modelCpp/BaseModel.cpp \
        modelCpp/ModelAp01.cpp \
        modelCpp/ModelGw01.cpp \
        modelCpp/ModelTh01.cpp \
        mqtt/BaseMqtt.cpp \
        mqtt/qmqtt_client.cpp \
        mqtt/qmqtt_client_p.cpp \
        mqtt/qmqtt_frame.cpp \
        mqtt/qmqtt_message.cpp \
        mqtt/qmqtt_network.cpp \
        mqtt/qmqtt_router.cpp \
        mqtt/qmqtt_routesubscription.cpp \
        mqtt/qmqtt_socket.cpp \
        mqtt/qmqtt_ssl_socket.cpp \
        mqtt/qmqtt_timer.cpp \
        mqtt/qmqtt_websocket.cpp \
        mqtt/qmqtt_websocketiodevice.cpp

RESOURCES += \
    files.qrc 
#    mainImage.qrc \
#    mainQml.qrc 

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    MainInterface.h \
    center/AccountMan.h \
    center/CenterMan.h \
    center/DecodeThread.h \
    drivers/DrvCommon.h \
    drivers/encrypt/mbedtls/aes.h \
    modelCpp/BaseModel.h \
    modelCpp/ModelAp01.h \
    modelCpp/ModelGw01.h \
    modelCpp/ModelTh01.h \
    mqtt/BaseMqtt.h \
    mqtt/qmqtt.h \
    mqtt/qmqttDepends \
    mqtt/qmqtt_client.h \
    mqtt/qmqtt_client_p.h \
    mqtt/qmqtt_frame.h \
    mqtt/qmqtt_global.h \
    mqtt/qmqtt_message.h \
    mqtt/qmqtt_message_p.h \
    mqtt/qmqtt_network_p.h \
    mqtt/qmqtt_networkinterface.h \
    mqtt/qmqtt_routedmessage.h \
    mqtt/qmqtt_router.h \
    mqtt/qmqtt_routesubscription.h \
    mqtt/qmqtt_socket_p.h \
    mqtt/qmqtt_socketinterface.h \
    mqtt/qmqtt_ssl_socket_p.h \
    mqtt/qmqtt_timer_p.h \
    mqtt/qmqtt_timerinterface.h \
    mqtt/qmqtt_websocket_p.h \
    mqtt/qmqtt_websocketiodevice_p.h \
    user_opt.h


#WIN32 
win32{
    message("is win32")
    RC_ICONS = logo.ico
}

##Android 
android{
message("is android")
QT += androidextras
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources
DISTFILES += \
    android-sources/AndroidManifest.xml
}









