import QtQuick 2.7
import "../base"
Rectangle {

    property var switchState: 0
    radius: 10
    width: parent.width
    height: 90
    
    ImageButton01
    {
        id: id_switchImage
        imgSrc: switchState>0 ? "qrc:/imagesRC/modelImages/on01.png" : "qrc:/imagesRC/modelImages/off01.png"
        height: parent.height*0.5
        width: height
        anchors
        {
            verticalCenter:parent.verticalCenter
            left:parent.left
            leftMargin:height*0.2
        }
        onSiqClickedLeft: 
        {
            theModelAp01.setSwitchState(!switchState)
        }
    }
    
    Connections
    {
        target: theModelAp01
        onSiqUpdateSwitchState:
        {
            switchState=state
        }
    }
    
}


