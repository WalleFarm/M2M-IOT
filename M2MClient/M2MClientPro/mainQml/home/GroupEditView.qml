import QtQuick 2.7
import QtQuick.Controls 2.0
import "../base"

//分组编辑界面

Rectangle {
    id:id_rootRect
    
    focus: true
    MsgDialog01
    { 
        id:id_msgDialog
    }
    
    Keys.onPressed: 
    {
        if(event.key === Qt.Key_Back)
        {
            console.log("phone Key_Back!")
            event.accepted = true;
            theAccountMan.setHomeCurrView("home-logined")
        }
    }
    
    ImageButton01//返回按钮
    {
        id:id_backButton
        source: "qrc:/imagesRC/mainImages/login/back.png"
        
        anchors
        {
            left:parent.left
            leftMargin:20
            top:parent.top
            topMargin:20            
        }
        onSiqClickedLeft: 
        {
            theAccountMan.setHomeCurrView("home-logined")
        }
    }
    ImageButton01{
        id:id_okButton
        source: "qrc:/imagesRC/mainImages/home/ok.png"
        height: 35
        anchors
        {
            right:parent.right
            rightMargin:20
            verticalCenter:id_backButton.verticalCenter
        }
        onSiqClickedLeft: //完成
        {
            var str_list=[]
            for(var i=0; i<id_listModel.count; i++)
            {
                str_list[i]=id_listModel.get(i).group_name
            }
            theCenterMan.requestOrderGroup(str_list)
        }
    }
    
    GroupRenameDialog//重命名
    {
        id:id_renameDialog
        onSiqOkClicked: 
        {
            if(text)
            {
                if(text==="全部")
                {
                    id_msgDialog.funOpen("不能使用\"全部\"作为组名!")
                    return
                }
                theCenterMan.requestRenameGroup(oldName, text)
            }
            funClose()
        }
    }
    GroupDelDialog //删除分组对话框
    {
        id:id_delGroupDialog
        onSiqOkClicked: 
        {
            theCenterMan.requestDelGroup(groupName)
            funClose()
        }
    }
    
    ListView{
        property int dragItemIndex: -1
        id:id_editListView
        clip: true
        width: parent.width
        anchors
        {
            top:id_backButton.bottom
            bottom:parent.bottom
        }
        model: ListModel{
            id:id_listModel
        }
        move:Transition {
            NumberAnimation { properties: "x,y"; duration: 200 }
        }
        moveDisplaced:Transition {
            NumberAnimation { properties: "x,y"; duration: 200 }
        }
        delegate: Rectangle{
            id:id_unionRect
            height: 60
            width: parent.width
            Rectangle
            {
                id:id_dragRect
                height: id_unionRect.height
                width: id_unionRect.width
                color: id_moveMouseArea.drag.active ? "transparent" : "white"
                MouseArea
                {
                    id:id_moveMouseArea
//                    enabled: group_name!=="全部"
                    anchors.fill: parent
                    drag.target: id_dragRect
                    drag.axis: Drag.YAxis
                    drag.onActiveChanged: {
                        if (id_moveMouseArea.drag.active) {
                            id_editListView.dragItemIndex = index;
//                            console.log("index=", index)
                        }
                        id_dragRect.Drag.drop();
                    }
                }
                states: [
                    State {
                        when: id_dragRect.Drag.active
                        ParentChange {
                            target: id_dragRect
                            parent: id_rootRect
                        }
    
                        AnchorChanges {
                            target: id_dragRect
                            anchors.horizontalCenter: undefined
                            anchors.verticalCenter: undefined
                        }
                    }
                ]
                Drag.active: id_moveMouseArea.drag.active
                Drag.hotSpot.x: id_dragRect.width / 2
                Drag.hotSpot.y: id_dragRect.height / 2
                
                ImageButton01{
                    id:id_delButton
                    source: "qrc:/imagesRC/mainImages/home/del.png"
                    height: 25
                    width: height
                    visible: group_name!=="全部"
                    anchors
                    {
                        verticalCenter:parent.verticalCenter
                        left:parent.left
                        leftMargin:20
                    }
                    onSiqClickedLeft: //删除
                    {
                        id_delGroupDialog.groupName=group_name
                        id_delGroupDialog.funOpen()
                    }
                }
                
                Text{  //组名文本
                    id:id_groupText
                    height: 40
                    anchors{
                        left: id_delButton.right
                        leftMargin: 10
                        right: id_totalNumText.left
                        verticalCenter: parent.verticalCenter
                    }
                    color: "black"
                    font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
                    font.pointSize: 18
                    font.bold: id_moveMouseArea.pressed
                    text: group_name
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    elide: Text.ElideRight 
                }
       
                Text{ //数量文本
                    id:id_totalNumText
                    height: 30
                    width: 30
                    anchors{
                        right: id_renameButton.left
                        rightMargin: 5
                        verticalCenter: parent.verticalCenter
                    }
                    color: "#808080"
                    font.family: "宋体"
                    font.pointSize: 18
                    text: total_num
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignLeft
                    elide: Text.ElideRight 
                }
                ImageButton01{ 
                    id:id_renameButton
                    source: "qrc:/imagesRC/mainImages/home/rename.png"
                    height: 25
                    width: height
                    visible: group_name!=="全部"     
                    anchors
                    {
                        verticalCenter:parent.verticalCenter
                        right:parent.right
                        rightMargin:20
                    }
                    onSiqClickedLeft: //重命名
                    {
                        id_renameDialog.oldName=group_name
                        id_renameDialog.funOpen(group_name)
                    }
                }
             
                
            }
           DropArea {
           id: dropArea
           anchors.fill: parent

           onDropped:{
//               console.log("onDropped")
               var other_index = id_editListView.indexAt(id_moveMouseArea.mouseX + id_unionRect.x, id_moveMouseArea.mouseY + id_unionRect.y);
//               console.log("index:",index,"other_index:",other_index,"listView.dragItemIndex:",id_editListView.dragItemIndex);
               if(other_index>=0)
               {
                   id_listModel.move(id_editListView.dragItemIndex,other_index, 1);
               }
           }
           
           Rectangle {
                   id: dropRectangle

                   anchors.fill: parent
                   color: "transparent"

                   states: [
                       State {
                           when: dropArea.containsDrag
                           PropertyChanges {
                                target: dropRectangle
                                color: "lightsteelblue"
                                opacity:0.3
                           }
                       }
                   ]
               }//end Rectangle

           }//end drop
            
           Rectangle{
                height: 1
                width: parent.width*0.7
                color: "#808080"
                anchors
                {
                    horizontalCenter:parent.horizontalCenter
                    bottom:parent.bottom
                }
//                visible: 
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
//            id_listModel.append({"group_name":"全部", "total_num":0})    
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
