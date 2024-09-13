import QtQuick 2.7
import "../base"

BaseSimpleView {
    property var switchState: 0
    property var onlineState: 0
    parent: theModelAp01.takeSimpleParent()
    imgSrc: "qrc:/imagesRC/modelImages/AP01.png"
    devName: theModelAp01.takeDeviceName()  
    dataValue: "离线" 
    anchors.fill: parent
    
    
    ImageButton01
    {
        id: id_switchImage
        imgSrc: switchState>0 ? "qrc:/imagesRC/modelImages/on02.png" : "qrc:/imagesRC/modelImages/off02.png"
        height: 35
        width: height
        anchors
        {
            top:parent.top
            topMargin:13
            right:parent.right
            rightMargin:13
        }
        onSiqClickedLeft: 
        {
            theModelAp01.setSwitchState(!switchState)
        }
    }
    
    
    Component.onCompleted: 
    {
//        console.log("parent=", parent)
    }
    
    Connections
    {
        target: theModelAp01
        onSiqUpdateDeviceName:
        {
            devName=dev_name
        }
        onSiqUpdateSensorValues:
        {
            if(onlineState>0)
            {
                dataValue="PM2.5 | "+pm25+" ug/m3" 
            }

        }
        onSiqUpdateOnlineState:
        {
            onlineState=state
            if(onlineState===0)
            {
                dataValue="离线"
            }
        }
        onSiqUpdateSwitchState:
        {
            switchState=state
        }
    }
}
