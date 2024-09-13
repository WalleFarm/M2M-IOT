import QtQuick 2.7 
import QtQuick.Controls 2.14 

Popup {
    
    property var titleText: "标题"
    signal siqCancelClicked()
    signal siqOkClicked(var text)
    id:id_popup
    visible: false
    implicitWidth: parent.width
    implicitHeight: 230
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
        width: parent.width
        anchors
        {
            top:parent.top
            topMargin:10
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: height*0.4
        font.family: "宋体"
        text: titleText
        color: "black"
    }
    
    BaseText01 
    { 
        id:id_inputText
        height: 50
        width: parent.width*0.8
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:id_titleText.bottom
            topMargin:10
        }
        placeholderText: "请输入内容"
        maximumLength: 30
        focus: true
    }
    
    BaseButton02//取消按钮
    { 
        id:id_cancelButton
        height: id_inputText.height
        width: id_inputText.width*0.4
        releaseColor: "#F0F0F0"
        pressedColor: "#F0F0F0"
        anchors
        {
            left:id_inputText.left
            top:id_inputText.bottom
            topMargin:20
        }
        buttonText: "取消"
        buttonColor:"#303030"
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
        anchors
        {
            verticalCenter:id_cancelButton.verticalCenter
            right:id_inputText.right
        }
        buttonText: "确定"
        
        onSiqClickedLeft: 
        {
            siqOkClicked(id_inputText.text)
        }
    }
    
    function funOpen(default_text)
    {
        id_inputText.text=default_text
        id_popup.open()
    }
    
    function funClose()
    {
        id_popup.close()
    }
}
