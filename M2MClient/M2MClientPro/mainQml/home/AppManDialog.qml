import QtQuick 2.7 
import QtQuick.Controls 2.14 
import "../base"
//应用管理弹框
 
Popup { 
    signal siqNewApp()
    signal siqRenameApp()
    property var rowHeight: 40
    id:id_popup
    visible: false
    implicitWidth: parent.width*0.6
    implicitHeight: rowHeight*(id_listModel.count+2)+id_popup.topPadding+id_popup.bottomPadding
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside
    x:10
    y:20 
    
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
        height: 40
        width: parent.width
        anchors
        {
            left:parent.left
            leftMargin:10
//            bottom:parent.bottom
            top:id_lineRect.bottom
        }
        textValue: "新建"
        onSiqClickedLeft: 
        {
            id_popup.close()
            siqNewApp()
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
    TextButton01//重命名按钮
    {
        id:id_renameButton
        height: id_newButton.height
        width: parent.width
        anchors
        {
            left:parent.left
            leftMargin:10
            top:id_newButton.bottom
        }
        textValue: "重命名"
        onSiqClickedLeft: 
        {
            id_popup.close()
            siqRenameApp()
        }
        Image//重命名图标
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
            source: "qrc:/imagesRC/mainImages/home/rename.png"
        }
    }
    Column
    {
        width: parent.width
        anchors
        {
            top:parent.top
            bottom:id_lineRect.top
        }
        spacing: 0
        Repeater
        {
            model: ListModel{
                id:id_listModel
            }
            Rectangle{
                height: rowHeight
                width: parent.width
                color: select ? "#c6ede8" :"transparent"
                radius: 5
                TextButton01//应用按钮
                {
                    id:id_appButton
                    height: 40
                    width: parent.width*0.8
                    anchors
                    {
                        left:parent.left
                        leftMargin:10
                        verticalCenter:parent.verticalCenter
                    }
                    textValue: app_name
                    onSiqClickedLeft: 
                    {
                        id_popup.close()
                        theCenterMan.selectCurrApp(app_id)
                    }
                    Image//打勾图标
                    {
                        id:id_selectImage
                        width: height
                        height: parent.height*0.8  
                        mipmap: true
                        visible:select
                        anchors
                        {
                            verticalCenter:parent.verticalCenter
                            left:parent.right
                        }
                        source: "qrc:/imagesRC/mainImages/home/select.png"
                    }
                }
            }
        }
    }
    
    
    Connections
    {
        target: theCenterMan
        onSiqUpdateAppListName:
        {
            if(index===0)
            {
                id_listModel.clear()
            }
            app_name=app_name+"("+app_id+")"
            id_listModel.append({"app_name":app_name, "app_id":app_id, "select":select})
        }
    }
    
}
