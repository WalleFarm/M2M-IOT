import QtQuick 2.7
import QtQuick.Controls 1.4
import "../base"
Rectangle
{
    property var devName: "设备名称"
    property var devSnStr: "11223344"
    property var headColor: "white"
    property var devNameColor: "black"
    property var devSnColor: "#808080"
    property var devStateColor: "#808080"
    property var headHeight: 50
    property var fillFlag: 1
    
    id:id_rootRect
    
//    anchors.fill: fillFlag>0 ? parent : null
    Rectangle
    {
        id:id_headRect
        width: parent.width
        height: headHeight
        color: headColor
        anchors
        {
            top:parent.top
            horizontalCenter:parent.horizontalCenter
        }

        Text{
            id:id_devNameText
            height: 30
            width: parent.width*0.5
            anchors
            {
                centerIn:parent
            }
            text: devName
            font.pointSize: 15
            font.bold: true
            font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
            color:  devNameColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        
        Label{ //显示设备SN
            id:id_devSnLabel
            height: 30
            width: 80
            anchors
            {
                right:parent.right
                rightMargin:10
                verticalCenter:parent.verticalCenter          
            }
             
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: height*0.5
            font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
            color: devSnColor
            text:devSnStr
        }
        
        Label{ //显示在线状态
            id:id_devStateLabel
            height: 30
            width: 80
            anchors
            {
                left:parent.left
                leftMargin:10
                verticalCenter:parent.verticalCenter          
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: height*0.5
            font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
            color: devStateColor
            text: "离线"
    }

    }
    
    Component.onCompleted: 
    {
        if(fillFlag>0)
            id_rootRect.anchors.fill=parent
    }
    function funSetDevState(dev_state)
    {
        if(dev_state>0)
            id_devStateLabel.text="在线"
        else
            id_devStateLabel.text="离线"
    }
    
}

