import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

TabBar {
    id:id_tabBar
    currentIndex: 0
    property var selectedTextColor: "#0C0C0C"
    property var releaseTextColor: "#6A6A6A"

    
    property var homeImgSrc0: "qrc:/imagesRC/mainImages/tabBar/home0.png"
    property var homeImgSrc1: "qrc:/imagesRC/mainImages/tabBar/home1.png"
    
    property var devImgSrc0: "qrc:/imagesRC/mainImages/tabBar/dev0.png"
    property var devImgSrc1: "qrc:/imagesRC/mainImages/tabBar/dev1.png"
    
    property var msgImgSrc0: "qrc:/imagesRC/mainImages/tabBar/msg0.png"
    property var msgImgSrc1: "qrc:/imagesRC/mainImages/tabBar/msg1.png"
    
    property var myImgSrc0: "qrc:/imagesRC/mainImages/tabBar/my0.png"
    property var myImgSrc1: "qrc:/imagesRC/mainImages/tabBar/my1.png"
    
    property var rowHeight: 24
    
    property var nameArray:["主页","设备","消息","我的"]
    Repeater
    {
        model: ListModel{
            id:id_listModel
        }
        
        
        TabButton//主页
        {
            Rectangle
            {
                color: "white"
                anchors.fill: parent
            }
            Image
            {
                id: id_tabImg
                width: height
                height: rowHeight
                mipmap: true
                anchors
                {
                    horizontalCenter: parent.horizontalCenter
                    top:parent.top
                    topMargin:height*0.2
                }
                source: funTakeImageSrc(tab_id)
            }
            Text
            {
                id: id_tabText
                text: nameArray[tab_id]
                anchors
                {
                    horizontalCenter:parent.horizontalCenter
                    top:id_tabImg.bottom
                }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignBottom
                color: (id_tabBar.currentIndex === tab_id) ? selectedTextColor : releaseTextColor
            }        
        }
    }
    
    Component.onCompleted: 
    {
        for(var i=0; i<4; i++)
        {
            id_listModel.append({"tab_id":i})
        }
    }
    
    function funTakeImageSrc(tab_id)
    {
        var is_select=tab_id===id_tabBar.currentIndex
        var img_src=homeImgSrc0;
        switch(tab_id)
        {
            case 0:
                img_src = is_select ? homeImgSrc1 : homeImgSrc0
                break;
            case 1:
                img_src = is_select ? devImgSrc1 :devImgSrc0
                break;
            case 2:
                img_src = is_select ? msgImgSrc1 :msgImgSrc0
                break;
            case 3:
                img_src = is_select ? myImgSrc1 :myImgSrc0
                break;
        }
        return img_src
    }
    
}
