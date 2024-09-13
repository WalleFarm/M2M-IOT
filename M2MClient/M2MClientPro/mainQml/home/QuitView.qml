import QtQuick 2.7
import QtQuick.Controls 2.0
import "../base"

//未登录home画面

Rectangle {
    
    gradient: Gradient {
            GradientStop { position: 0.0; color: "#CFD5E6" }
            GradientStop { position: 1.0; color: "#F5F6F8" }
        }
    
    //LOGO
    Image
    {
        id: id_logoImg
        width: height
        height: 40
        mipmap: true
        anchors
        {
            left:parent.left
            leftMargin:width*0.3
            top:parent.top
            topMargin:height*0.2
        }
        source: "qrc:/imagesRC/mainImages/logo.png"
    }
    Label{
        id:id_headLabel
        height: id_logoImg.height
        width: height*4
        anchors
        {
            verticalCenter:id_logoImg.verticalCenter
            left:id_logoImg.right
            leftMargin:10
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pointSize: height*0.40
        font.family:"宋体"
        color: "black"
        text:"端点物联M2M Lite"
    }
    
    Label{
        id:id_myDevLabel
        height: id_logoImg.height
        width: height*4
        anchors
        {
            bottom:id_centerRect.top
            bottomMargin:5
            left:id_centerRect.left
            leftMargin:10
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pointSize: height*0.45
        font.family:"宋体"
        color: "black"
        text:"我的设备"
    }
    
    Rectangle //中心登录空白矩形
    {
        id:id_centerRect
        
        width:parent.width*0.9
        
        radius:10
        anchors
        {
            top:id_logoImg.bottom
            topMargin:id_logoImg.height*2
            horizontalCenter:parent.horizontalCenter
            bottom:parent.bottom
            bottomMargin:20
        }
        
        Image //冰箱等智能家居设备图片
        {
            id: id_devicesImg
            width: parent.width*0.8
            height: width/1.6
            mipmap: true
            anchors
            {
                horizontalCenter:parent.horizontalCenter
                top:parent.top
                topMargin:parent.height*0.08
            }
            source: "qrc:/imagesRC/mainImages/home/home_devices.png"
        }
        
        
        BaseButton02
        {
            height: 50
            width: 150
            buttonText: "前往登录"
            anchors
            {
                horizontalCenter:parent.horizontalCenter
                bottom:parent.bottom
                bottomMargin:height*3
            }
            onSiqClickedLeft: 
            {
                theAccountMan.setViewByLoginState(0)
            }
        }
    }
    
    

}
