import QtQuick 2.7

Text {//文字
    property var textValue: "状态"
    property var textColor: "black"
    signal siqClickedLeft()
    id:id_rootText
    height: 35
    width: 60
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignLeft
    font.pointSize: height*0.4
    text: textValue
    color: textColor
    elide: Text.ElideRight 
    
    MouseArea
    {
        anchors.fill: parent
        onPressed: 
        {
            id_rootText.font.bold=true
        }
        onClicked: 
        {
            id_rootText.font.bold=false
            siqClickedLeft()
        }
    }
}
