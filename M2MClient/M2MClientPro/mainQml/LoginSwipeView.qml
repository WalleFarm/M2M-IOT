import QtQuick 2.7
import QtQuick.Controls 2.0

import "login"
//登录切换界面

SwipeView {
    signal siqLoginBackLevel1()
    
    id: id_loginSwipeView

    interactive: false//禁用手滑切换
    
    PhoneLogin  //验证码登录
    {
        
        onSiqLoginBackLevel0: //返回按钮
        {
            siqLoginBackLevel1()
        }
        
        onSiqGotoPasswdLogin:
        {
            id_loginSwipeView.currentIndex=1//跳转到密码登录页面
        }
    }   
    
    PasswdLogin  //密码登录
    {
        
        onSiqPasswdBackLevel0:
        {
            id_loginSwipeView.currentIndex=0//返回到验证码登录页面
        }
    }
}
