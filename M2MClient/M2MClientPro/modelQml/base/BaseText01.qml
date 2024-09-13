import QtQuick 2.7
import QtQuick.Controls 2.14
TextField {
    property var borderColor: "#0ee7cb"
    height: 35
    width: 200
    background: Rectangle{ 
        color: "#F0F0F0"
        border.width: 2
        border.color: parent.focus ? borderColor : "white"
        radius: parent.height*0.3
    }
     
    color: "black"
    font.family: "宋体"
    font.pointSize: height*0.3
    placeholderText: "请输入内容"
}
