import QtQuick 2.7
import QtQuick.Controls 2.0
import "../base"

//注册帐号页面

Rectangle {
    signal siqGoBackLevel0() 
    MsgDialog01 
    {
        id:id_msgDialog
    }
    
    Keys.onPressed: 
    {
        if(event.key === Qt.Key_Back)
        {
            console.log("reg Key_Back!")
            event.accepted = true;
            siqGoBackLevel0()
        }
    }
    
    ImageButton01
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
            siqGoBackLevel0()
        }
    }
    
    HeadView 
    {
        id:id_headView
        textValue: "帐号注册"
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:parent.top
            topMargin:60
        }
    }
    BaseText01
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
        placeholderText: "请输入用户名"
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
        placeholderText: "输入密码"
        maximumLength: 30
        echoMode: TextInput.Password//密码模式
    }
    
    BaseText01//确认密码
    {
        id:id_confirmText
        height: id_accountText.height
        width: id_accountText.width
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_passwdText.bottom
            topMargin:10
        }
        placeholderText: "确认密码"
        maximumLength: 30
        echoMode: TextInput.Password//密码模式
    }
    
    BaseText01
    {
        id:id_phoneText
        height: 50
        width: parent.width*0.8
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_confirmText.bottom
            topMargin:10
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
    
    UserAgreement//隐私政策
    {
        id:id_userAgreement
        anchors
        {
            left:id_verCodeView.left
            top:id_verCodeView.bottom
            topMargin:10
        }
    }
    
    BaseButton02//注册按钮
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
        buttonText: "立即注册"
        
        onSiqClickedLeft: 
        {
            if(id_userAgreement.checkState==0)
            {
                id_msgDialog.funOpen("请先同意用户协议!")
                return
            }
            var result=theAccountMan.checkAccount(id_accountText.text)
            if(result===1)
            {
                id_msgDialog.funOpen("账户名长度不能小于5!")
                return
            }
            else if(result===2)
            {
                id_msgDialog.funOpen("不能以admin作为账户名!")
                return
            }
            else if(result===3)
            {
                id_msgDialog.funOpen("不能纯数字作为账户名!")
                return
            }
            result=theAccountMan.checkPasswd(id_passwdText.text)
            if(result===1)
            {
                id_msgDialog.funOpen("密码长度要大于8!")
                return 
            }
            if(id_passwdText.text!==id_confirmText.text)
            {
                id_msgDialog.funOpen("两次密码不一致!")
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
            
            theAccountMan.requestReg(id_accountText.text, id_passwdText.text, id_phoneText.text, id_verCodeView.verCode, )
        }
    }
    
}
