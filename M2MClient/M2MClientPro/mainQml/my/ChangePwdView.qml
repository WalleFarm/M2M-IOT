import QtQuick 2.7
import QtQuick.Controls 2.0
import "../base"
import "../login"
//修改密码页面

Rectangle {
    signal siqGoBackLevel0() 
    property var accountName: "user"
    MsgDialog01 
    {
        id:id_msgDialog
    }
    Keys.onPressed: 
    {
        if(event.key === Qt.Key_Back)
        {
            console.log("forget Key_Back!")
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
        textValue: "修改密码"
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
        text:accountName
        readOnly: true
        maximumLength: 30
    }
    BaseText01//旧密码
    {
        id:id_oldText
        height: id_accountText.height
        width: id_accountText.width
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_accountText.bottom
            topMargin:10
        }
        placeholderText: "旧密码"
        maximumLength: 30
        echoMode: TextInput.Password//密码模式
    }
    BaseText01//密码
    {
        id:id_newText
        height: id_accountText.height
        width: id_accountText.width
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_oldText.bottom
            topMargin:10
        }
        placeholderText: "新密码"
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
            top:id_newText.bottom
            topMargin:10
        }
        placeholderText: "确认密码"
        maximumLength: 30
        echoMode: TextInput.Password//密码模式
    }
    
    

    BaseButton02//修改按钮
    {
        id:id_loginButton
        height: id_newText.height
        width: id_newText.width
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_confirmText.bottom
            topMargin:10
        }
        buttonText: "立即修改"
        
        onSiqClickedLeft: 
        {

            var result=theAccountMan.checkPasswd(id_newText.text)
            if(result===1)
            {
                id_msgDialog.funOpen("密码长度要大于8!")
                return 
            }
            if(id_newText.text!==id_confirmText.text)
            {
                id_msgDialog.funOpen("两次密码不一致!")
                return
            }   
            theAccountMan.requestChangePwd(id_accountText.text, id_oldText.text, id_newText.text)
        }
    }
    
}
