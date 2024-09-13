import QtQuick 2.7
import QtQuick.Controls 2.0
import "../base"

//验证码登录界面

Rectangle {
    signal siqLoginBackLevel0()
    signal siqGotoPasswdLogin()
    MsgDialog01
    {
        id:id_msgDialog
    }
    
    Keys.onPressed: 
    {
        if(event.key === Qt.Key_Back)
        {
            console.log("phone Key_Back!")
            event.accepted = true;
            siqLoginBackLevel0()
        }
    }
    
    ImageButton01//返回按钮
    {
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
            siqLoginBackLevel0()
        }
    }
    
    HeadView
    {
        id:id_headView
        textValue: "手机验证码登录"
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:parent.top
            topMargin:80
        }
    }
    
    BaseText01
    {
        id:id_phoneText
        height: 50
        width: parent.width*0.8
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_headView.bottom
            topMargin:30
        }
        placeholderText: "请输入手机号"
        maximumLength: 11
    }
    
    VerCodeView//验证码模块
    {
        id:id_verCodeView
        height: id_phoneText.height
        agreeCheckState: id_userAgreement.checkState
        phoneNumber: id_phoneText.text
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_phoneText.bottom
            topMargin:10
        }
    }
    
    BaseButton02
    {
        id:id_loginButton
        height: id_phoneText.height
        width: id_phoneText.width
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_verCodeView.bottom
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
            var ok=theAccountMan.isPhoneNumber(id_phoneText.text)
            if(!ok)
            {
                id_msgDialog.funOpen("请输入正确的手机号!")
                return
            }
            ok=theAccountMan.isNumber(id_verCodeView.verCode)
            if(ok!==4)
            {
                id_msgDialog.funOpen("请输入正确的验证码!")
                return
            }
            theAccountMan.requestLoginByVerCode(id_phoneText.text, id_verCodeView.verCode)
        }
    }
    
    UserAgreement//隐私政策
    {
        id:id_userAgreement
        anchors
        {
            left:id_phoneText.left
            top:id_loginButton.bottom
            topMargin:10
        }
    }
    
    BaseButton02
    {
        id:id_switchButton
        height: id_phoneText.height
        width: id_phoneText.width
        
        releaseColor: "#F0F0F0"
        pressedColor: "#E0E0E0"
        buttonColor:"#505050"
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_userAgreement.bottom
            topMargin:30
        }
        buttonText: "密码登录>>"
        
        onSiqClickedLeft: 
        {
            siqGotoPasswdLogin()
        }
    }
    
}
