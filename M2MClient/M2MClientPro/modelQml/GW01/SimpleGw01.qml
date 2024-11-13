import QtQuick 2.7
import "../base"

BaseSimpleView {
    property var onlineState: 0
    parent: theModelGw01.takeSimpleParent()
    imgSrc: "qrc:/imagesRC/modelImages/GW01.png"
    devName: theModelGw01.takeDeviceName()  
    dataValue: "离线" 
    anchors.fill: parent
    
    Component.onCompleted: 
    {
        console.log("load simple gw 01 ok!")
    }
    
    Connections
    {
        target: theModelGw01
        onSiqUpdateDeviceName:
        {
            devName=dev_name
        }
        onSiqUpdateOnlineState:
        {
            onlineState=state
            if(onlineState===0)
            {
                dataValue="离线"
                dataColor="#808080"
            }
            else
            {
                dataValue="在线"
                dataColor="#202020"     
            }
        }
    }
}
