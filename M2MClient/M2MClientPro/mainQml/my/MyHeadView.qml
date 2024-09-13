import QtQuick 2.7
import QtGraphicalEffects 1.0
Rectangle {
    property var levelValue: 0
    property var loginState: 1
    property var rowHeight: 35
    property var accountText: "admin"
    width: parent.width
    height: 180
    gradient: Gradient 
    {
            GradientStop { position: 0.0; color: "#CFD5E6" }
            GradientStop { position: 1.0; color: "#F5F6F8" }
        }
    
    //头像
    Rectangle{
        id:id_headRect
        width: height  
        height: parent.height*0.4
        anchors
        {
            left:parent.left
            leftMargin:parent.height*0.15
            top:parent.top
            topMargin:parent.height*0.15
        }
        color: "white"
        visible: false
        radius: height/2
        border.width: 1
        border.color: "black"
    }
    Image
    {
        id: id_headImg
        mipmap: true
        anchors.centerIn: parent
        visible: false
        smooth: true
        source: loginState ? "qrc:/imagesRC/mainImages/my/head1.png" : "qrc:/imagesRC/mainImages/my/head0.png"
    }
    OpacityMask {
        anchors.fill: id_headRect
        source: id_headImg
        maskSource: id_headRect
       }


    //账户名
    Text//账户文本
    {
        id:id_accountText;
        height: rowHeight
        width: parent.width*0.7
        anchors
        {
            top:id_headRect.top
            left:id_headRect.right
            leftMargin:rowHeight*0.3
        }
        font.pointSize: id_accountMouseArea.pressed ? height*0.5 : height*0.45
        font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
        font.bold:true
        text: loginState ? accountText : "点击登录"
        color: "black"
        verticalAlignment:Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        MouseArea{
            id:id_accountMouseArea
            anchors.fill: parent
            enabled: !loginState
            onClicked: 
            {
                theAccountMan.setViewByLoginState(0)
            }
        }
    }
    Text//等级文本
    {
        id:id_levelText;
        height: rowHeight*0.8
        width: parent.width*0.7
        anchors
        {
            top:id_accountText.bottom
            left:id_accountText.left
        }
        font.pointSize: height*0.45
        font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
        text: "等级: "+levelValue
        color: "black"
        verticalAlignment:Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
    }
}
















