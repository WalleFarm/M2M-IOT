import QtQuick 2.7 
import QtQuick.Controls 2.14 

Popup {
    
    property var titleText: "标题"
    property var cancelText: "取消"
    property var okText: "确定"
    property var cancelColor: "#303030"
    property var okColor: "#303030"
    
    signal siqCancelClicked()
    signal siqOkClicked()
    id:id_popup
    visible: false
    implicitWidth: parent.width
    implicitHeight: 200
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside
    anchors.centerIn: Overlay.overlay
    
    enter: Transition {
              NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
          }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0 }
    }
    background: Rectangle
    {
        radius:10
    }
    
    Text {//标题文字
        id:id_titleText
        height: 40
        width: parent.width*0.8
        anchors
        {
            top:parent.top
            topMargin:20
            horizontalCenter:parent.horizontalCenter
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 14
        font.family: "宋体"
        lineHeight: 1.5
        text: titleText
        color: "black"
    }
    
    
    BaseButton02//取消按钮
    { 
        id:id_cancelButton
        height: 50
        width: parent.width*0.4
        releaseColor: "#F0F0F0"
        pressedColor: "#F0F0F0"
        anchors
        {
            left:parent.left
            leftMargin:20
            bottom:parent.bottom
            bottomMargin:20
        }
        buttonText: cancelText
        buttonColor:cancelColor
        onSiqClickedLeft: 
        {
            siqCancelClicked()
        }
    }
    BaseButton02//确定按钮
    { 
        id:id_okButton
        height: id_cancelButton.height
        width: id_cancelButton.width
        releaseColor: "#F0F0F0"
        pressedColor: "#F0F0F0"
        anchors
        {
            verticalCenter:id_cancelButton.verticalCenter
            right:parent.right
            rightMargin:20
        }
        buttonText: okText
        buttonColor:okColor
        onSiqClickedLeft: 
        {
            siqOkClicked()
        }
    }
    
    function funOpen()
    {
        id_popup.open()
    }
    
    function funClose()
    {
        id_popup.close()
    }
}
