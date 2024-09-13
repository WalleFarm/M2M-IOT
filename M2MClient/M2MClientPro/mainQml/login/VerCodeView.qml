import QtQuick 2.7
//import QtQuick.Controls 2.0
import "../base"

//获取验证码视图模块

Rectangle{
    height: 50
    width: parent.width*0.8
    property var agreeCheckState: 0
    property var phoneNumber: ""
    property var verCode: id_codeText.text

    MsgDialog01
    {
        id:id_msgDialog
    }
    
    BaseText01
    {
        id:id_codeText
        height: parent.height
        width: parent.width*0.6
        anchors
        {
            left:parent.left
            verticalCenter:parent.verticalCenter
        }
        placeholderText: "验证码"
        maximumLength: 4
    }
    
    BaseButton02
    {
        property var timeValue: 0
        id:id_getCodeButton
        height: id_codeText.height
        anchors
        {
            verticalCenter:id_codeText.verticalCenter
            left:id_codeText.right
            leftMargin:5
            right:parent.right
        }
        buttonText: "获取"
        
        onSiqClickedLeft: 
        {
            if(timeValue<=0)//已获取,等待超时
            {
                if(agreeCheckState==0)
                {
                    id_msgDialog.funOpen("请先同意用户协议!")
                    return
                }
                //验证输入的手机号是否正确
                var ok=theAccountMan.isPhoneNumber(phoneNumber)
                if(!ok)
                {
                    id_msgDialog.funOpen("请输入正确的手机号!")
                    return
                }
                timeValue=30
                theAccountMan.requestVerCode(phoneNumber, phoneNumber)
            }
        }
        Timer {
            interval: 1000; running: true; repeat: true
            onTriggered: 
            {
                if(id_getCodeButton.timeValue>0)
                {
                    id_getCodeButton.timeValue-=1
                    if(id_getCodeButton.timeValue>0)
                    {
                        id_getCodeButton.buttonText=id_getCodeButton.timeValue+"s"
                    }
                    else
                    {
                        id_getCodeButton.buttonText="获取"
                    }
                }
            }
        }
    }

}
