import QtQuick 2.7
import QtQuick.Controls 1.4
import "../base"

Rectangle {
    property var nodeSn: "12345678"
    MsgDialog01 
    {
        id:id_msgDialog  
    }    
    signal siqGotoGwView();
    Rectangle
    {
        id:id_headRect
        width: parent.width
        height: 50
        anchors
        {
            top:parent.top
        }
        ImageButton01//返回
        {
            id:id_backButton
            source: "qrc:/imagesRC/modelImages/back.png"
            width:30
            anchors
            {
                left:parent.left
                leftMargin:20
                verticalCenter:id_headRect.verticalCenter
            }
            onSiqClickedLeft: 
            {
                siqGotoGwView();
            }
        }
        Label{ //节点SN
            id:id_nodeSnLabel
            height: 30
            width: 60
            anchors
            {
                verticalCenter:parent.verticalCenter
                horizontalCenter:parent.horizontalCenter
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.pointSize: height*0.5
            font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
            color: "black"
            text: nodeSn
        }
        
        BaseButton02//导出按钮
        {
            id:id_exportButton
            buttonText:"导出"
            width:80
            anchors
            {
                verticalCenter:id_nodeSnLabel.verticalCenter
                right:parent.right
                rightMargin:10
            }
            onSiqClickedLeft:
            {
                var ack_str=theModelGw01.exportNode01History(nodeSn)
                id_msgDialog.funOpen(ack_str)
            }
        }
     }//end
        
    ListView
    {
        clip: true
        width: parent.width
        spacing: 2
        anchors
        {
            top:id_headRect.bottom
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
            height: 40
            anchors.horizontalCenter: parent.horizontalCenter
            
            Label{ //数据显示
                id:id_dataLabel
                height: 30
                anchors
                {
                    left:parent.left
                    leftMargin: 20
                    right:parent.right
                    verticalCenter:parent.verticalCenter
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
        

    
//    Component.onCompleted: 
//    {
//        for(var i=0; i<10; i++)
//        {
//            id_listModel.append({"data_str":"123456789"})
//        }
//    }
    
    Connections
    {
        target: theModelGw01
      
        onSiqAddNode01History:
        {
            if(id===0)
            {
                id_listModel.clear()
            }
            id_listModel.append({"data_str":data_str})
        }
    }
    
}
