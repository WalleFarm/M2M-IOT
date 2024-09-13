import QtQuick 2.7 
import QtQuick.Controls 2.14 
import "../base"
//分组管理弹框

Popup { 
    signal siqNewGroup()
    signal siqManGroup()
    signal siqOpenGroup(var id, var group_name)
    property var rowHeight: 40
    id:id_popup
    visible: false
    implicitWidth: parent.width*0.6
    implicitHeight: rowHeight*(id_listModel.count+2)+id_popup.topPadding+id_popup.bottomPadding
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside
    
    x:parent.width*0.3
    y:10
    
//    enter: Transition {
//              NumberAnimation { property: "height"; from: rowHeight; to: rowHeight*(id_listModel.count+2)+id_popup.topPadding+id_popup.bottomPadding }
//          }
    enter: Transition {
              NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
          }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 }
    }
    background: Rectangle
    {
        
        radius:15
    }
    Rectangle//分割横线
    {
        id:id_lineRect
        width: parent.width*0.8
        height: 1
        color:"#F0F0F0"
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            bottom:parent.bottom
            bottomMargin:rowHeight*2
        }
    }
    TextButton01//新建按钮
    {
        id:id_newButton
        height: rowHeight
        width: parent.width
        anchors
        {
            left:parent.left
            leftMargin:10
            top:id_lineRect.bottom
        }
        textValue: "新建"
        onSiqClickedLeft: 
        {
            id_popup.close()
            siqNewGroup()
        }
        Image//新建图标
        {
            width: height
            height: parent.height*0.6  
            mipmap: true
            anchors
            {
                verticalCenter:parent.verticalCenter
                right:parent.right
                rightMargin:15
            }
            source: "qrc:/imagesRC/mainImages/home/new.png"
        }
    }
    
    TextButton01//分组管理
    {
        id:id_manButton
        height: id_newButton.height
        width: parent.width
        anchors
        {
            left:parent.left
            leftMargin:10
            top:id_newButton.bottom
        }
        textValue: "管理"
        onSiqClickedLeft: 
        {
            id_popup.close() 
            siqManGroup()
        } 
        Image//管理图标
        {
            width: height
            height: parent.height*0.5  
            mipmap: true
            anchors
            {
                verticalCenter:parent.verticalCenter
                right:parent.right
                rightMargin:17
            }
            source: "qrc:/imagesRC/mainImages/home/man.png"
        }
    }
    
    ListView
    {
        clip: true
        width: parent.width
        anchors
        {
            top:parent.top
            bottom:id_lineRect.top
        }
        model: ListModel{
            id:id_listModel
        }
        
        delegate: Rectangle{
            height: rowHeight
            width: parent.width
            TextButton01//分组按钮
            {
                id:id_groupButton
                height: rowHeight
                width: parent.width-20
                anchors
                {
                    left:parent.left
                    leftMargin:10
                    verticalCenter:parent.verticalCenter
                }
                textValue: group_name
                onSiqClickedLeft: //打开某个分组
                {
                    id_popup.close()
                    siqOpenGroup(index, group_name)
                }
                Text{
                    id:id_totalNumText
                    height: parent.height
                    width: 30
                    anchors{
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    color: "#808080"
                    font.family: "宋体"
                    font.pointSize: 18
                    text: total_num
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            Rectangle{
                height: 1
                width: parent.width*0.7
                color: "#D0D0D0"
                visible: index!==id_listModel.count-1
                anchors
                {
                    horizontalCenter:parent.horizontalCenter
                    bottom:parent.bottom
                }
            }
        }
    }
    
    Component.onCompleted: 
    {
//          for(var i=0; i<1; i++)
//          {
//              id_listModel.append({"id":i, "group_name":"全部", "total_num":0})
//          }
    }
    
    Connections
    {
        target: theCenterMan
        onSiqClearGroup:
        {
            id_listModel.clear()  
        }
        onSiqAddGroup:
        {
              id_listModel.append({"group_name":group_name, "total_num":total_num})
        }
        onSiqUpdateGroupTotalNum:
        {
            for(var i=0; i<id_listModel.count; i++)
            {
                if(group_name===id_listModel.get(i).group_name)
                {
                    id_listModel.setProperty(i, "total_num", total_num)
                }
            }
        }
        onSiqRenameGroup:
        {
            for(var i=0; i<id_listModel.count; i++)
            {
                var group_name=id_listModel.get(i).group_name
                if(group_name===old_name)
                {
                    id_listModel.setProperty(i, "group_name", new_name)
                    break
                }
            }
        }
        onSiqDelGroup:
        {
            for(var i=0; i<id_listModel.count; i++)
            {
                if(group_name===id_listModel.get(i).group_name)
                {
                    id_listModel.remove(i)
                    break
                }
            }
        }
    }
    
}
