import QtQuick 2.7
import QtQuick.Controls 1.4

import "../base"
ScrollView {
    signal sigUnionClicked(var index)
    property var rowHeight: 60
    property var rectWidth: parent.width*0.92
    width: parent.width*0.92
    height: rowHeight*5
    clip: true
    verticalScrollBarPolicy:Qt.ScrollBarAlwaysOff
    
    
    Rectangle
    {
        radius: 10
        height: rowHeight*7
        width: rectWidth
        
        Column
        {
            anchors.fill: parent
            
            Repeater
            {
                model: ListModel{
                    id:id_listModel
                }
                
                delegate: Rectangle{
                     radius: 10
                    height: rowHeight
                    width: parent.width
                    color: id_mouseArea.pressed ? "lightblue" : "white"
                    Rectangle//横线
                    {
                        visible: index>0
                        width: parent.width*0.9
                        height: 1
                        color: "#CFD5E6"
                        anchors
                        {
                            top:parent.top
                            horizontalCenter:parent.horizontalCenter
                        }
                    }
                    
                    Image {  //头部图标
                        id: id_headImage
                        source: img_src
                        height: 25
                        width: height
                        mipmap: true
                        anchors
                        {
                            left:parent.left
                            leftMargin:15
                            verticalCenter:parent.verticalCenter
                        }
                    }
                    Text//头部文本
                    {
                        id:id_headText;
                        height:30
                        width: parent.width*0.7
                        anchors
                        {
                            verticalCenter:parent.verticalCenter
                            left:id_headImage.right
                            leftMargin:10
                        }
                        font.pointSize: height*0.45
                        font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
                        text: head_text
                        color: "black"
                        verticalAlignment:Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                    }
                    Image {  //箭头图标
                        id: id_nextImage
                        source: "qrc:/imagesRC/mainImages/my/into.png"
                        height: 25
                        width: height
                        mipmap: true
                        visible: index<3
                        anchors
                        {
                            right:parent.right
                            rightMargin:15
                            verticalCenter:parent.verticalCenter
                        }
                    }
                    MouseArea
                    {
                        id:id_mouseArea
                        anchors.fill: parent
                        onClicked: 
                        {
                            sigUnionClicked(index)
                        }
                    }
                    
                }
            }
            
        }
        
        BaseButton02//退出登录
        { 
            id:id_quitButton
            height: 40
            width: parent.width*0.6
            releaseColor: "#F0F0F0"
            pressedColor: "#F0F0F0"
            visible:myLoginState
            anchors
            {
                bottom:parent.bottom
                bottomMargin:15
                horizontalCenter:parent.horizontalCenter
            }
            buttonText: "退出登录"
            buttonColor:"#F00000"
            border.color:buttonColor
            onSiqClickedLeft: 
            {
                id_myQuitDialog.funOpen()
            }
        }
    }
    
    

    
    Component.onCompleted: 
    {
        var img_src="qrc:/imagesRC/mainImages/my/passwd.png"
        id_listModel.append({"img_src":img_src, "head_text":"修改密码"})
        img_src="qrc:/imagesRC/mainImages/my/phone.png"
        id_listModel.append({"img_src":img_src, "head_text":"联系人"})
        img_src="qrc:/imagesRC/mainImages/my/child.png"
        id_listModel.append({"img_src":img_src, "head_text":"子账户"})
        img_src="qrc:/imagesRC/mainImages/my/update.png"
        id_listModel.append({"img_src":img_src, "head_text":"软件更新"})
        img_src="qrc:/imagesRC/mainImages/my/about.png"
        id_listModel.append({"img_src":img_src, "head_text":"关于"})
    }
    
}
