import QtQuick 2.7
import "../base"
Rectangle {
    
    width: parent.width*0.92
    height: 120
    radius: 10
    
    MsgDialog01 
    { 
        id:id_msgDialog
    }
    
    ImageTextButton01
    { 
        textColor: "black"
        textValue: "交流中心" 
        imgSrc: "qrc:/imagesRC/mainImages/my/share.png"
        rowHeight: 20
        height: 45
        anchors
        {
            left:parent.left
            leftMargin:width*0.5
            verticalCenter:parent.verticalCenter
        }
        onSiqClickedLeft: 
        {
//            id_msgDialog.funOpen("此功能暂未开放!")
            Qt.openUrlExternally("https://blog.csdn.net/ypp240124016/category_12653350.html")
        }
    }
    ImageTextButton01  
    {
        textColor: "black"
        textValue: "商城中心"
        imgSrc: "qrc:/imagesRC/mainImages/my/shop.png"
        rowHeight: 20
        height: 45
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            verticalCenter:parent.verticalCenter
        }
        onSiqClickedLeft: 
        {
            Qt.openUrlExternally("https://blog.csdn.net/ypp240124016/category_12631333.html")
        }
    }
    ImageTextButton01
    {
        textColor: "black"
        textValue: "充值中心"
        imgSrc: "qrc:/imagesRC/mainImages/my/money.png"
        rowHeight: 20
        height: 45
        anchors
        {
            right:parent.right
            rightMargin:width*0.5
            verticalCenter:parent.verticalCenter
        }
        onSiqClickedLeft: 
        {
            Qt.openUrlExternally("https://blog.csdn.net/ypp240124016/category_12609773.html")
        }
    }
}
