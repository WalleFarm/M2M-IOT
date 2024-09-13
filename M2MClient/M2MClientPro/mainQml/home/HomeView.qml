import QtQuick 2.7
import QtQuick.Controls 2.0

import "../base"

SwipeView {
    id:id_homeSwipeView
    interactive: false//禁用手滑切换
    currentIndex: 0
    LoginView//已登录画面
    {
        visible:id_homeSwipeView.currentIndex==0
    }
    
    QuitView//未登录画面
    {
        visible:id_homeSwipeView.currentIndex==1
    }
    
    GroupEditView //分组管理画面
    {
        visible:id_homeSwipeView.currentIndex==2
    }
    
    Connections
    {
        target: theAccountMan
        onSiqSetHomeCurrView:
        {
            console.log("HomeView_name=", view_name)
            if(view_name=="home-logined")
            {
                id_homeSwipeView.currentIndex=0
            }
            else if(view_name=="home-quit")
            {
                id_homeSwipeView.currentIndex=1
            }
            else if(view_name=="home-group")
            {
                id_homeSwipeView.currentIndex=2
            }
        }

    }
}
