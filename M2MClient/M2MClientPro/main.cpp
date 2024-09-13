#include <QGuiApplication>
#include<QApplication>
#include <QQmlApplicationEngine>
#include "MainInterface.h"

#if defined(Q_OS_ANDROID)
#include<QtAndroid>
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    
    qRegisterMetaType<u8>("u8");  
    qRegisterMetaType<u16>("u16");  
    qRegisterMetaType<u32>("u32");  
    
    //启动画面
#if defined(Q_OS_ANDROID)
        QTimer::singleShot(2000,NULL,[=](){
            QtAndroid::hideSplashScreen(1000);
        });
#endif
        
    MainInterface mainInterface;
    
    return app.exec();
}
