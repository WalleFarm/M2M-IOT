import QtQuick 2.7
import "../base"

BaseSimpleView {
    property var onlineState: 0
    parent: theModelTh01.takeSimpleParent()
    imgSrc: "qrc:/imagesRC/modelImages/TH01.png"
    devName: theModelTh01.takeDeviceName()  
    dataValue: "离线" 
    anchors.fill: parent
    
    
    Connections
    {
        target: theModelTh01
        onSiqUpdateDeviceName:
        {
            devName=dev_name
        }
        onSiqUpdateSensorValues:
        {
            if(onlineState>0)
            {
                dataValue= temp+"℃ | "+humi+"%" 
                var alarm=temp_alarm>0 || humi_alarm
                dataColor=alarm ? "red" : "#808080"
            }

        }
        onSiqUpdateOnlineState:
        {
            onlineState=state
            if(onlineState===0)
            {
                dataValue="离线"
                dataColor="#808080"
            }
        }
    }
}
