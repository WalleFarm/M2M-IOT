import QtQuick 2.7
import QtQuick.Controls 2.0
import "../base"
//账号密码登录页面
//还有注册和忘记密码两个切换页面
    
SwipeView {
    signal siqPasswdBackLevel0()
    
    id:id_passwdSwipeView
    interactive: false//禁用手滑切换
    MsgDialog01
    {
        id:id_msgDialog
    }
    Keys.onPressed: 
    {
        if(event.key === Qt.Key_Back)
        {
            console.log("passwd Key_Back!")
            event.accepted = true;
            siqPasswdBackLevel0()
        }
    }
    Rectangle{ //账号密码登录页面
        ImageButton01//返回到验证码登录页面
        {
            id:id_backButton
            source: "qrc:/imagesRC/mainImages/login/back.png"
            anchors
            {
                left:parent.left
                leftMargin:20
                top:parent.top
                topMargin:20
            }
            onSiqClickedLeft: 
            {
                siqPasswdBackLevel0()
            }
        }
        HeadView
        {
            id:id_headView
            textValue: "密码登录"
            anchors
            {
                horizontalCenter:parent.horizontalCenter
                top:parent.top
                topMargin:60
            }
        }
        
        BaseText01//账户
        {
            id:id_accountText
            height: 50
            width: parent.width*0.8
            anchors
            {
                horizontalCenter:parent.horizontalCenter
                top:id_headView.bottom
                topMargin:30
            }
            placeholderText: "用户名/手机号"
            maximumLength: 30
        }
        BaseText01//密码
        { 
            id:id_passwdText
            height: id_accountText.height
            width: id_accountText.width
            anchors
            {
                horizontalCenter:parent.horizontalCenter
                top:id_accountText.bottom
                topMargin:10
            }
            placeholderText: "密码"
            maximumLength: 30
            echoMode: TextInput.Password//密码模式
        }
        
        UserAgreement//隐私政策
        {
            id:id_userAgreement
            anchors
            {
                left:id_passwdText.left
                top:id_passwdText.bottom
                topMargin:10
            }
        }
        
        BaseButton02//登录按钮
        {
            id:id_loginButton
            height: id_passwdText.height
            width: id_passwdText.width
            anchors
            {
                horizontalCenter:parent.horizontalCenter
                top:id_userAgreement.bottom
                topMargin:10
            }
            buttonText: "登录"
            
            onSiqClickedLeft: 
            {
                if(id_userAgreement.checkState==0)
                {
                    id_msgDialog.funOpen("请先同意用户协议!")
                    return
                }
                theAccountMan.requestLogin(id_accountText.text, id_passwdText.text, 1)
            }
        }
        
        Rectangle{  //注册+忘记密码  行
            
            width: parent.width*0.55
            height: 40
            anchors
            {
                horizontalCenter:parent.horizontalCenter
                top:id_loginButton.bottom
                topMargin:30
            }
            TextButton01{
                id:id_regText
                textValue: "注册"
                textColor: "#303030"
                
                anchors
                {
                    verticalCenter:parent.verticalCenter
                    left:parent.left
                }
                onSiqClickedLeft: 
                {
                    id_passwdSwipeView.currentIndex=1//跳转页面
                }
            }
            TextButton01{
                id:id_forgetText
                textValue: "忘记密码"
                textColor: "#303030"
                
                anchors
                {
                    verticalCenter:parent.verticalCenter
                    right:parent.right
                }
                onSiqClickedLeft: 
                {
                    id_passwdSwipeView.currentIndex=2//跳转页面
                }
            }
        }

    }
    
    RegView //账号注册
    {
        
        onSiqGoBackLevel0:
        { 
            id_passwdSwipeView.currentIndex=0
        }
    }
    
    ForgetView  //忘记密码
    {
        onSiqGoBackLevel0:
        { 
            id_passwdSwipeView.currentIndex=0
        }
    }
    
}
    
   




