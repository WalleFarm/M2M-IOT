import QtQuick 2.6
import QtQuick.Controls 2.12

Popup { 
    property var rowHeight: 40
    property var bgColor: "#404040"
    property var pressedColor: "white"
    property var isOpened: false
    
    signal siqClickedLeft(var row_id)
    id:id_popup
    visible: false
//    implicitWidth: 320
//    implicitHeight: implicitWidth*0.618
//    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside
    
    background: Rectangle
    {
        color:"transparent"
    }
    Rectangle
    {
        radius: 10
        anchors.fill: parent
        color: bgColor
    }
    ListView
    {
        id:id_listView  
        clip: true
        anchors.centerIn: parent
        width: parent.width-10
        height: parent.height-20
        delegate:Rectangle{
            id:id_itemRect
            width: id_listView.width
            height: rowHeight
            color: bgColor
            radius: 2
            Image {
                id: id_headerImg            
                height: imgSrc ? parent.height*0.618 : parent.height*0.1
                width: height
                mipmap: true
                anchors
                {
                    left:parent.left
                    leftMargin:width*0.3
                    verticalCenter:parent.verticalCenter
                }
                source: imgSrc          
            }
            Label{ //名称
                id:id_nameLabel
                height: parent.height*0.8
                anchors
                {
                    left:id_headerImg.right
                    leftMargin:parent.height*0.1
                    right:parent.right
                    rightMargin:parent.height*0.1
                    verticalCenter:parent.verticalCenter
                }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: height*0.45
                font.family:"宋体"
                color: textColor
                text:textValue
            }
            MouseArea
            {
                anchors.fill: parent
                onClicked: 
                {
                    id_itemRect.color=bgColor
                    id_nameLabel.font.bold=false
                    siqClickedLeft(id)
                    funCloseDialog()
                }
                onPressed: 
                {
                    id_itemRect.color=pressedColor
                    id_nameLabel.font.bold=true
                }
                onCanceled: 
                {
                    id_itemRect.color=bgColor
                    id_nameLabel.font.bold=false
                }
            }
        }
        
        model:ListModel 
        {
            id:id_listModel
        }
    }
    
    function funOpenDialog()
    {
        height=rowHeight*id_listModel.count+40
        isOpened=true
        id_popup.open()
    }
    
    function funCloseDialog()
    {
        isOpened=false
        id_popup.close()
    }
      
    function funAddItem(imgSrc, textValue,  textColor)
    {
        id_listModel.append({"id":id_listModel.count, "imgSrc":imgSrc, "textValue":textValue, "textColor":textColor})
    }
    
}
