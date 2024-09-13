import QtQuick 2.7
import QtQuick.Controls 1.4
import "../base"

BaseModelView
{
    id:id_baseModelView
    parent: theModelAp01.takeModelParent()
    
    devName: theModelAp01.takeDeviceName()
    devSnStr: theModelAp01.takeDevSnStr()
    
    ScrollView {
        id:id_zoneView
        implicitWidth: 400
        width: parent.width
        clip: true
        anchors
        {
            top:parent.top
            topMargin:50
            bottom:parent.bottom
        }
        verticalScrollBarPolicy:Qt.ScrollBarAlwaysOff
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        Rectangle
        {
            id:id_centerRect
            width: parent.width
            height: 800
            color: "#FAFAFA"
            Ap01CenterShow//中心圆圈画面
            {
                id:id_airCenterShow
                width: parent.width
                height: parent.height*0.6
                anchors
                {
                    top:parent.top
                }
            }
    
            Ap01SwitchItem//开关栏
            {
                id:id_switchItem
                width: parent.width*0.96
                anchors
                {
                    horizontalCenter:parent.horizontalCenter
                    top:id_airCenterShow.bottom
                }
            } 
            
            Ap01ControlItem //风速控制
            {
                id:id_airControlItem
                anchors
                {
                    top:id_switchItem.bottom
                    topMargin:5
                    horizontalCenter:parent.horizontalCenter
                }
            }
        }
        
        
    }
    
    Component.onCompleted: 
    {
        id_zoneView.width=width
        id_centerRect.width=width
//        console.log("width=", width)
    }
    
    Connections
    {
        target: theModelAp01
        onSiqUpdateSensorValues:
        {
            id_airCenterShow.funUpdateThText(temp, humi)//更新温湿度数值
            id_airCenterShow.funUpdatePm25Text(pm25)//更新PM2.5数值
        }
        onSiqUpdateOnlineState:
        {
            funSetDevState(state)//更新在线状态
        }
        onSiqUpdateDeviceName:
        {
            devName=dev_name
        }
    }
    
}


