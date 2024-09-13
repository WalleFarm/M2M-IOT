import QtQuick 2.0
import QtQuick.Controls 2.5
Popup { 
    property var textValue: "状态"
    property var textColor: "black"
    property var timeValue: 0
    id:id_popup
    visible: false
    implicitWidth: 200
    implicitHeight: 50 
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside
    anchors.centerIn: Overlay.overlay
    enter: Transition {
              NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
          }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 }
    }
    background: Rectangle
    {
        opacity:0.9
        color:"#F0F0F0"
        radius:10
    }
    
    Text {//文字
        id:id_msgText
        height: 35
        width: 60
        anchors.centerIn: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: height*0.4
        font.family: "宋体"
        text: textValue
        color: textColor
    }
    
    Timer {
        interval: 1000; running: true; repeat: true
        onTriggered: 
        {
             if(timeValue>0)
             {
                timeValue--
                 if(timeValue<=0)
                 {
                    funClose()
                 }
             }
        }
    }
    function funOpen(msg)
    {
        textValue=msg
        id_popup.width=id_msgText.contentWidth+30
        id_popup.open()
        timeValue=2
    }
    
    function funClose()
    {
        id_popup.close()
    }
    
}
