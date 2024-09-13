import QtQuick 2.7 
import QtQuick.Controls 2.14 
import "../base"
//分组管理弹框

Popup { 
    signal siqMoveGroup(var group_name)
    property var rowHeight: 40
    property var moveDevList: []
    property var srcGroupName: ""
    id:id_popup
    visible: false
    implicitWidth: parent.width*0.6
    implicitHeight: rowHeight*(id_listModel.count)+id_popup.topPadding+id_popup.bottomPadding
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside
    
    x:parent.width*0.3
    y:10
      

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
    
    
    ListView
    {
        clip: true
        width: parent.width
        anchors
        {
            top:parent.top
            bottom:parent.bottom
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
                    siqMoveGroup(group_name)
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
