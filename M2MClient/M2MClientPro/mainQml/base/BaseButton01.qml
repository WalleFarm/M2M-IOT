import QtQuick 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4



Rectangle
{
    property var buttonText: "按钮名称"
    property var buttonColor: "white"
    property var buttonFontFamily: "黑体"
    property var pressedColor: Qt.rgba(36/255, 146/255, 255/255, 0.5)
    property var releaseColor: Qt.rgba(36/255, 146/255, 255/255)
    signal siqClickedLeft()
    
    id:id_rootRect
    height: 30
    width: 80
    border.width: 1
    border.color: "white"
    radius: 2
    color: releaseColor
    Label{
        id:id_btnLabel
        anchors.fill: parent
        anchors.margins: 2
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: height*0.35
        font.family:buttonFontFamily
        color: buttonColor
        text:buttonText
    }
    MouseArea
    {
        anchors.fill: parent
        onClicked: 
        {
            id_btnLabel.font.bold=false
            id_rootRect.color=releaseColor
            siqClickedLeft()
        }
        onCanceled: 
        {
            id_btnLabel.font.bold=false
            id_rootRect.color=releaseColor
        }
        onPressed: 
        {
            id_btnLabel.font.bold=true
            id_rootRect.color=pressedColor
        }
    }
}
