import QtQuick 2.7
import QtQuick.Controls 2.0
import "base"
//主页面,可以在控制中心和登录页面之间切换

SwipeView {
    
    
    id:id_mainSwipeView
    anchors.fill: parent
    
    interactive: false//禁用手滑切换
    currentIndex: 0
     
    MsgDialog01 
    {
        id:id_msgDialog
    }
    property var pressTime: 0
    Timer {
              interval: 500; running: true; repeat: true
              onTriggered: 
              {
                    if(pressTime>0)
                    {
                        pressTime--
                    }
              }
    }
    Keys.onPressed: 
    {
        if(event.key === Qt.Key_Back)
        {
            console.log("login Key_Back!")
            if(pressTime>0)
            {
                console.log("app quit!")
                Qt.quit()
            }
            else
            {
                pressTime=2
                event.accepted = true;
                id_msgDialog.funOpen("再按一次退出!")                
            }

        }
    }
    
    CenterSwipeView //控制中心界面
    {
        id:id_centerSwipeView
        visible:id_mainSwipeView.currentIndex==0
    }
    
    LoginSwipeView  //登录界面
    {
        id:id_loginSwipeView
        visible:id_mainSwipeView.currentIndex==1
        onSiqLoginBackLevel1://登录界面返回按钮
        {
            id_mainSwipeView.currentIndex=0
        }
    }
    
    Component.onCompleted: 
    {

    }
    
    Connections
    {
        target: theAccountMan
        onSiqSetMainCurrView:
        {
            console.log("MainView_name=", view_name)
            if(view_name=="main-center")
            {
                id_mainSwipeView.currentIndex=0
            }
            else if(view_name=="main-login")
            {
                id_mainSwipeView.currentIndex=1
            }
        }
        onSiqShowMsg:
        {
            id_msgDialog.funOpen(msg)
        }
    }
    Connections
    {
        target: theCenterMan
        onSiqShowMsg:
        {
            id_msgDialog.funOpen(msg)
        }
    }

    function funSetFocus()
    {
        focus=true
    }
    
}
