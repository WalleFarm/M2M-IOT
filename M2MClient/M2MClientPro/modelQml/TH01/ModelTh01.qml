import QtQuick 2.7
import QtQuick.Controls 1.4
import "../base"

BaseModelView
{
    id:id_baseModelView
    parent: theModelTh01.takeModelParent()
    
    devName: theModelTh01.takeDeviceName()
    devSnStr: theModelTh01.takeDevSnStr()
    headColor: "#1296db"
    devNameColor: "white"
    devSnColor: "#F0F0F0"
    devStateColor: "#F0F0F0"
    
    property var setOpenType: 0  //0--温度设置  1--湿度设置
    
    MsgDialog01
    {
        id:id_msgDialog
    }
    Th01SetDialog
    {
        id:id_threshSetDialog
        onSiqOkClicked: 
        {
            if(setOpenType===0)
            {
                var ack_str=theModelTh01.setTempThresh(text)
            }
            else
            {
                ack_str=theModelTh01.setHumiThresh(text)
            }
            funClose()
            if(ack_str)
            {
                id_msgDialog.funOpen(ack_str)
            }
        }
    }
    
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
            gradient: Gradient //渐变色
            {
                GradientStop { position: 0.0; color: "#1296db" }
                GradientStop { position: 1.0; color: "#0ee7cb" }
            }
            
            Th01DataItem//温度模块
            {
                id:id_tempItem
                anchors
                {
                    horizontalCenter:parent.horizontalCenter
                    top:parent.top
                    topMargin:50
                }
                onSiqThreshClicked: 
                {
                    setOpenType=0
                    id_threshSetDialog.titleText="高温预警值(℃)"
                    id_threshSetDialog.funOpen(threshText)
                }
            }
            
            Th01DataItem//湿度模块
            {
                id:id_humiItem
                anchors
                {
                    horizontalCenter:parent.horizontalCenter
                    top:id_tempItem.bottom
                    topMargin:50
                }
                headText: "湿度 | 正常"
                valueText:"50.5"
                unionText:"%"
                threshText:"95.2"
                onSiqThreshClicked: 
                {
                    setOpenType=1
                    id_threshSetDialog.titleText="高湿预警值(%)"
                    id_threshSetDialog.funOpen(threshText)
                }
            }
            
            Rectangle//横线
            {
                width: parent.width*0.6
                height: 1
                anchors
                {
                    horizontalCenter:parent.horizontalCenter
                    top:id_tempItem.bottom
                    topMargin:25
                }
                opacity: 0.5
            }

        }

    }
    
    Component.onCompleted: 
    {
        id_zoneView.width=width
        id_centerRect.width=width
        
        theModelTh01.requestThreshValue()//界面显示后请求阈值数据
    }
    
    Connections
    {
        target: theModelTh01
        onSiqUpdateSensorValues://数据更新
        {            
            id_tempItem.valueText=temp
            if(temp_alarm>0)//温度报警
            {
                id_tempItem.headText="温度 | 高温"
                id_tempItem.valueColor="red"
            }
            else
            {
                id_tempItem.headText="温度 | 正常"
                id_tempItem.valueColor="white"
            }
            
            id_humiItem.valueText=humi
            if(humi_alarm>0)//湿度报警
            {
                id_humiItem.headText="湿度 | 潮湿"
                id_humiItem.valueColor="red"
            }
            else
            {
                id_humiItem.headText="湿度 | 正常"
                id_humiItem.valueColor="white"               
            }
        }
        onSiqUpdateThresh://阈值更新
        {
            id_tempItem.threshText=temp
            id_humiItem.threshText=humi;
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


