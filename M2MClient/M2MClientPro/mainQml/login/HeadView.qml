import QtQuick 2.7
import QtQuick.Controls 2.0

//LOGO+文字说明

Rectangle {
    property var logoSrc: "qrc:/imagesRC/mainImages/logo.png"
    property var textValue: "标题文字"
    color: "transparent"
    width: parent.width
    height: 100
    //LOGO
    Image
    {
        id: id_logoImg
        width: height
        height: 60
        mipmap: true
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:parent.top
        }
        source: logoSrc
    }
    Label{
        id:id_titleLabel
        height: 30
        width: height*4
        anchors
        {
            horizontalCenter:id_logoImg.horizontalCenter
            top:id_logoImg.bottom
            topMargin:10
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: height*0.45
        font.family:"宋体"
        color: "black"
        text:textValue
    }
}
