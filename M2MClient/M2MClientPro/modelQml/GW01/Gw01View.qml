import QtQuick 2.7
import QtQuick.Controls 1.4

import "../base"

BaseModelView
{
    id:id_baseModelView
    fillFlag: 0
    
    devName: theModelGw01.takeDeviceName()
    devSnStr: theModelGw01.takeDevSnStr()
    
    signal siqGotoNodeView(var index);
    MsgDialog01 
    {
        id:id_msgDialog  
    } 
    
    ListView
    {
        clip: true
        width: parent.width
        spacing: 5
        anchors
        {
            top:parent.top
            topMargin:headHeight
            bottom:parent.bottom
        }
        model: ListModel{
            id:id_listModel
        }
        
        delegate: Rectangle{
            
            border.width: 1
            border.color: "#F0F0F0"
            radius: 2
            width: parent.width*0.95
            height: 70
            anchors.horizontalCenter: parent.horizontalCenter
            
            Label{ //ID号
                id:id_indexLabel
                height: 30
                width: 40
                anchors
                {
                    top:parent.top
                    topMargin:5
                    left:parent.left
//                    leftMargin:2
                }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
                font.pointSize: height*0.5
                font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
                color: "black"
                text:index+1+"."
            }
            
            Label{ //节点SN
                id:id_nodeSnLabel
                height: 30
                width: 60
                anchors
                {
                    verticalCenter:id_indexLabel.verticalCenter
                    left:id_indexLabel.right
                    leftMargin:5
                }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                font.pointSize: height*0.5
                font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
                color: "black"
                text:node_sn
            }
            
            Label{ //节点信号  RSSI+SNR
                id:id_rssiSnrLabel
                height: 30
                width: 60
                anchors
                {
                    verticalCenter:id_indexLabel.verticalCenter
                    left:id_nodeSnLabel.right
                    leftMargin:5
                    right:id_detailButton.left
//                    rightMargin:2
                }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: height*0.4
                font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
                color: "black"
                text:rssi+"dbm,"+snr+"dbm"
            }
            
            BaseButton02//设备详情按钮
            {
                id:id_detailButton
                buttonText:"历史>>"
                width:80
                anchors
                {
                    verticalCenter:id_nodeSnLabel.verticalCenter
                    right:parent.right
                    rightMargin:10
                }
                onSiqClickedLeft:
                {
                    siqGotoNodeView(index)
                    id_node01View.nodeSn=node_sn
                    theModelGw01.updateNode01History(node_sn)//请求历史数据
                }
            }
            BaseButton02
            {
                id:id_downButton
                buttonText:"下发"
                width:80
                anchors
                {
                    horizontalCenter:id_detailButton.horizontalCenter
                    verticalCenter:id_dataLabel.verticalCenter
                }
                onSiqClickedLeft:
                {
                    theModelGw01.setNode01DownMsg(node_sn)//下发数据
                }
            }
            Label{ //数据显示
                id:id_dataLabel
                height: 30
//                width: parent.width*0.95
                anchors
                {
                    bottom:parent.bottom
                    bottomMargin:5
                    left:parent.left
                    leftMargin: 20
                    right:parent.right
                }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                font.pointSize: height*0.45
                font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
                color: "black"
                text:data_str
            }
            
        }
        
    }  
    
    Component.onCompleted: 
    {
        for(var i=0; i<10; i++)
        {
//          id_listModel.append({"id":i+1, "node_sn":"A1011234", "rssi":"-145", "snr":"-11", "data_str":"1234567890"})
        }
    }
    
    Connections
    {
        target: theModelGw01
      
        onSiqUpdateOnlineState:
        {
            funSetDevState(state)//更新在线状态
        }
        onSiqUpdateDeviceName:
        {
            devName=dev_name
        }
        onSiqShowMsg:
        {
            id_msgDialog.funOpen(msg)
        }
        onSiqUpdateNode01Data:
        {
            for(var i=0; i<id_listModel.count; i++)
            {
                if(id_listModel.get(i).node_sn===sn_str)
                {
                    id_listModel.setProperty(i, "rssi", rssi_str)
                    id_listModel.setProperty(i, "snr", snr_str)
                    id_listModel.setProperty(i, "data_str", data_str)
                    return;
                }
            }
            id_listModel.append({"node_sn":sn_str, "rssi":rssi_str, "snr":snr_str, "data_str":data_str})
        }
    }
}
