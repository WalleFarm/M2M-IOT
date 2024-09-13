import QtQuick 2.7

Item {
    property var imgSrc: ""
    property var devName: "设备名称"
    property var dataValue: "000"
    property var dataColor: "#808080"
    width: 150
    height: 120
    Image
    {
        id: id_headImg
        width: height  
        height: parent.height*0.5
        mipmap: true
        source: imgSrc
        anchors
        {
            left:parent.left
            leftMargin:5
            top:parent.top
            topMargin:5
        }
    }
    Text{
        id:id_devNameText
        height: 25
        anchors
        {
            bottom:id_dataText.top
            left:id_dataText.left
            right:parent.right
        }
        text: devName
        font.pointSize: 14
//        font.bold: true
        font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
        color:  "black"
    }
    Text{
        id:id_dataText
        height: 25
        anchors
        {
            bottom:parent.bottom
            left:parent.left
            leftMargin:10
            right:parent.right
        }
        text: dataValue
        font.pointSize: 12
        font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
        color: dataColor
    }
    
}
