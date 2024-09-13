import QtQuick 2.7
import "../base"
//已登录home画面

Rectangle 
{
    property var netState: 0
    property var currAppID: 0
    property var currAppName: "当前应用"
    gradient: Gradient 
    {
            GradientStop { position: 0.0; color: "#CFD5E6" }
            GradientStop { position: 1.0; color: "#F5F6F8" }
        }
    
    TextButton01  //当前项目名称
    { 
        id:id_appManButton
        height: 40
        width: parent.width*0.6
        anchors
        {
            left:parent.left
            leftMargin:10
            top:parent.top
            topMargin:10
        }
        textValue: currAppName  
        onSiqClickedLeft: 
        {
            id_appManDialog.open()
            theCenterMan.updateAppListName()//更新应用列表名称
        }
    }
    Image//向下图标
    {
        width: height
        height: id_appManButton.height*0.5  
        mipmap: true
        anchors
        {
            verticalCenter:id_appManButton.verticalCenter
            left:id_appManButton.left
            leftMargin:id_appManButton.contentWidth+10
        }
        source: "qrc:/imagesRC/mainImages/home/down.png"
    }
    
    Image//网络状态图标
    {
        id:id_netImage
        width: height
        height: 30
        mipmap: true
        anchors
        {
            verticalCenter:id_appManButton.verticalCenter
            right:id_addDevImg.left
            rightMargin:10
        }
        source: "qrc:/imagesRC/mainImages/home/net.png"
        Timer {
                  interval: 1000; running: !netState; repeat: true
                  onTriggered: 
                  {
                        id_netImage.visible=!id_netImage.visible
                  }
                  onRunningChanged: 
                  {
                        if(!running)
                        {
                            id_netImage.visible=false
                        }
                  }
                }
    }
    
    
    AppManDialog//项目管理弹框
    { 
        id:id_appManDialog
        
        onSiqNewApp:
        {
            id_appNewDialog.funOpen("新应用")
        }
        onSiqRenameApp:
        {
            id_appRenameDialog.funOpen(currAppName)
        }
    }
    AppNewDialog//新建应用编辑框
    {
        id:id_appNewDialog
    }
    AppRenameDialog//应用重命名编辑框
    {
        id:id_appRenameDialog
        onSiqOkClicked:  
        {
            if(text)
            {
                theCenterMan.requestRenameApp(currAppID, text)
            }
             funClose()
        }
    }
    
    GroupTabView
    { 
        id:id_groupTabview
        width: parent.width
        anchors
        {
            top:id_appManButton.bottom
            topMargin:10
            bottom:parent.bottom
        } 
    }
    
    DevAddDialog
    {
        id:id_devAddDialog
        onSiqOkClicked: 
        {
            var result=theCenterMan.requestAddDeviceQml(text)
            if(result)
            {
                id_msgDialog.funOpen(result)
            }
            funClose()
        }
    }
    
    CameraView//扫码相机
    {
        id:id_cameraView
    }
    MiniDialog01//添加设备迷你框
    {
        id:id_addMiniDialog
        width: id_addDevImg.width*5
        height: rowHeight*2+50
        x:id_addDevImg.x-width*0.7
        y:id_addDevImg.y+id_addDevImg.height*0.65
        bgColor: "#303030"
        pressedColor: Qt.rgba(0.8, 0.8, 0.8, 0.5)
        Component.onCompleted: 
        {
            funAddItem("qrc:/imagesRC/mainImages/home/manual.png", "手动添加", "white")
            funAddItem("qrc:/imagesRC/mainImages/home/scan.png", "扫码添加", "white")
        }
        onSiqClickedLeft: 
        {
//                console.log("row_id=", row_id)
            switch(row_id)
            {
                case 0:
                    id_devAddDialog.funOpen("")
                    break
                case 1:
                    id_cameraView.funOpenView()//打开扫码框
                    break
            }
        }
    }
    ImageButton01//添加图标
    {
        id: id_addDevImg
        width: height
        height: 30
        anchors
        {
            verticalCenter:id_appManButton.verticalCenter
            right:parent.right
            rightMargin:20
        }
        source: "qrc:/imagesRC/mainImages/home/new.png"  
        onSiqClickedLeft: 
        {
            id_addMiniDialog.funOpenDialog()
        }
    }
    
    Connections
    {
        target: theCenterMan
        onSiqUpdateCurrAppName:
        {
//            console.log("curr app id=", app_id)
//            console.log("curr app name=", app_name)
            currAppID=app_id
            currAppName=app_name
        }
    }
    Connections
    {
        target: theMainInterface
        onSiqUpdateNetState:
        {
            netState=state
        }
    }
}
