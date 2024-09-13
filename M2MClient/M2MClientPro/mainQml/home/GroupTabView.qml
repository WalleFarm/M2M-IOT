import QtQuick 2.7
import QtQuick.Controls 2.12
import "../base"

//首页的分组管理

Rectangle
{
    color: "transparent"
    MsgDialog01 
    {
        id:id_msgDialog
    }
    ListView//滑动tab bar
    {
        id:id_tabBarView
        clip:true
        orientation: ListView.Horizontal
        height: 40
        anchors
        {
            left:parent.left
            right:id_listButton.left
        }
        model: ListModel{
            id:id_tabBarModel
        }
        delegate: Rectangle{
            height: 40
            width: id_titleText.contentWidth+15
            color: "transparent"
            Text{
                id:id_titleText
                height: parent.height
                width: 20
                anchors.centerIn: parent
                font.family: Qt.platform.os === "windows" ? "宋体" : "黑体"
                font.pointSize: 18
                font.bold: id_tabBarView.currentIndex===index
                text: group_name
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            MouseArea
            {
                anchors.fill: parent
                onClicked: 
                {
                    id_tabBarView.currentIndex=index
                }
            }
        }
        onCurrentIndexChanged: 
        {
            id_mainView.currentIndex=currentIndex
        }
    }
    
    GroupNewDialog//新建分组对话框
    {
        id:id_groupNewDialog
    }
    
    GroupManDialog//分组管理对话框
    {
        id:id_groupManDialog
        onSiqNewGroup: //新建分组
        {
            id_groupNewDialog.funOpen("新分组")
        }
        onSiqManGroup: //分组管理
        {
            theAccountMan.setHomeCurrView("home-group")
        }
        onSiqOpenGroup: 
        {
            id_tabBarView.currentIndex=id
            id_mainView.currentIndex=id
        }
    }
    DevMoveGroupDialog//移动设备分组列表
    {
        id:id_devMoveDialog 
        x:parent.width*0.3
        y:parent.height-height-60
        onSiqMoveGroup:
        {
//            console.log("dev list=", moveDevList)
//            console.log("src group=", srcGroupName)
//            console.log("dst group=", group_name)
            if(srcGroupName===group_name)
            {
                id_msgDialog.funOpen("请移动到其它分组!")
                return
            }
            if(moveDevList.length<=0)
            {
                id_msgDialog.funOpen("未选择设备!")
                return
            }
            theCenterMan.requestMoveDevice(srcGroupName, group_name, moveDevList)
        }
        
    }
    
    Rectangle{  //列表按钮
        id:id_listButton
        height: id_tabBarView.height*0.8
        width: height*1.5
        anchors
        {
            right:parent.right
            rightMargin:15
            verticalCenter:id_tabBarView.verticalCenter
        }
        
        radius: height/2
        color: "#BFC5D6"
        ImageButton01{
            anchors.centerIn: parent
            height: parent.height*0.85
            width: height
            source: "qrc:/imagesRC/mainImages/home/list.png"
            onSiqClickedLeft: 
            {
                id_groupManDialog.open()
            }
        }
    }

    SwipeView {  //分组面切换
         id: id_mainView
         width: parent.width
         anchors
         {
            top:id_tabBarView.bottom
            bottom:parent.bottom
         }
         Repeater {
             model: ListModel{
             id:id_mainModel
             }
    
             Rectangle {
              color: "transparent"
              
              Text{
                id:id_onlineText
                height: 30
                anchors
                {
                    top:parent.top
                    topMargin:15
                    left:parent.left
                    leftMargin:5
                    right:parent.right    
                }
                font.pointSize: 15
                font.family: "Times New Roman"
                text: "在线设备 "+online_text
              }
              
              
              DevGridView
              {
                  groupName: group_name
//                  devMoveGroupDialog: id_devMoveDialog
                    anchors
                    {
                        top:id_onlineText.bottom
                        bottom:parent.bottom
                    }
              }
              
//              Text{
//                anchors.centerIn: parent
//                font.pointSize: 18
//                text: index+group_name
//              }
          }
         }
         onCurrentIndexChanged: {
             id_tabBarView.currentIndex=currentIndex
         }
     }
      Component.onCompleted: 
      {

      }
    
      Connections
      {
          target: theCenterMan
          onSiqClearGroup://清除分组
          {
              id_tabBarModel.clear()
              id_mainModel.clear()

          }
          onSiqAddGroup://添加分组
          {
                id_tabBarModel.append({"group_name":group_name})
                id_mainModel.append({"group_name":group_name, "online_text":"0/0"})
          }

          onSiqRenameGroup://重命名分组
          {
              for(var i=0; i<id_tabBarModel.count; i++)
              {
                  var group_name=id_tabBarModel.get(i).group_name
                  if(group_name===old_name)
                  {
                      id_tabBarModel.setProperty(i, "group_name", new_name)
                      id_mainModel.setProperty(i, "group_name", new_name)
                      break
                  }
              }
          }
          onSiqDelGroup://删除分组
          {
              for(var i=0; i<id_tabBarModel.count; i++)
              {
                  if(group_name===id_tabBarModel.get(i).group_name)
                  {
                      id_tabBarModel.remove(i)
                      id_mainModel.remove(i)
                      break
                  }
              }
          }
          onSiqUpdateOnlineText:
          {
              for(var i=0; i<id_tabBarModel.count; i++)
              {
                  if(group_name===id_tabBarModel.get(i).group_name)
                  {
                      id_mainModel.setProperty(i, "online_text", online_text)
                      break
                  }
              }
          }
      }
}





