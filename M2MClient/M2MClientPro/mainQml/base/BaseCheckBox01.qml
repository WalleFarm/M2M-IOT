import QtQuick 2.7
import QtQuick.Controls 2.14
import QtQuick.Controls.Styles 1.4

CheckBox {
    property var checkedImage: ""
    property var checkedText: "复选框文本"
    id:id_rootCheckBox
    height: 35
    width: 80
    
    indicator:Rectangle{
        height: parent.height*0.9
        width: height
        
        radius: height/2
        color: "#F0F0F0"
        anchors.verticalCenter: id_rootCheckBox.verticalCenter
        Image
        {
            mipmap: true
            anchors.fill: parent
            source: id_rootCheckBox.checkState>0 ? checkedImage : ""
        }
        
    }
    
    contentItem: Text {
        id:id_text
        height: id_rootCheckBox.height
        width: contentWidth
        anchors.verticalCenter: id_rootCheckBox.verticalCenter
        text: checkedText
        font.family: "宋体"
        font.pointSize: height*0.6
        color: "#303030"
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        leftPadding: id_rootCheckBox.height
    }
    
    onCheckStateChanged: 
    {
        console.log("state=",checkState)
    }
}
