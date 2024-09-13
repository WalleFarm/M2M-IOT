import QtQuick 2.7

Image
{
    property var rowHeight: 30
    property var imgSrc: ""
    property var textValue: "状态"
    property var textColor: "white"
    
    signal siqClickedLeft()
    id: id_btnImg
    width: height  
    height: 30  
    mipmap: true
    source: imgSrc
    
    Text {//文字
        height: rowHeight
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:parent.bottom
            topMargin:height*0.3
        }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: height*0.5
        text: textValue
        color: textColor
    }
    MouseArea
    {
        anchors.fill: parent
        onPressed: 
        {
            id_btnImg.scale=1.1
        }
        onExited: 
        {
            id_btnImg.scale=1.0
        }
        onClicked: 
        {
            id_btnImg.scale=1.0
            siqClickedLeft()
        }
    }
}
