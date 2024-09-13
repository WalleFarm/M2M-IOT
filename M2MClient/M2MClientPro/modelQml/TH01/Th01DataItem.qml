import QtQuick 2.7
import "../base"

//数据显示单元
Item {
    
    signal siqThreshClicked()
    property var headText: "温度 | 正常"
    property var valueText: "26.3"
    property var valueColor: "white"
    property var unionText: "℃"
    property var threshText: "50.0"
    implicitWidth: 300
    implicitHeight: 200
    
    
    Text{
        id:id_headText //头部标题
        height: 40
        width: 160
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:parent.top
        }
        text: headText
        font.pointSize: 18
        font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
        color: "white"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    
    Text{
        id:id_valueText //数值
        height: 100
        width: 150
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_headText.bottom
            topMargin:10
        }
        text: valueText
        font.pointSize: 50
        font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
        color: valueColor
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    
    Text{
        id:id_unionText //单位
        height: 40
        width: 60
        anchors
        {
            right:id_headText.right
            top:id_valueText.top
        }
        text: unionText
        font.pointSize: 15
        font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
        color: "white"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
    }
    
    Text{
        id:id_threshText //阈值
        height: 40
        width: 40
        anchors
        {
            horizontalCenter:id_valueText.horizontalCenter
            top:id_valueText.bottom
        }
        text: threshText
        font.pointSize: 15
        font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
        font.bold: id_mouseArea.pressed
        color: "white"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
        MouseArea
        {
            id:id_mouseArea
            anchors.fill: parent
            onClicked: 
            {
                siqThreshClicked()
            }
        }
    }
    Text{
        id:id_union2Text //单位
        height: 40
        width: 60
        anchors
        {
            left:id_threshText.right
            verticalCenter:id_threshText.verticalCenter
        }
        text: unionText
        font.pointSize: 15
        font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
        color: "white"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
    }
    
}
