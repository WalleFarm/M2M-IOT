import QtQuick 2.7

Image
{
    signal siqClickedLeft()
    property var mouseX: id_mouseArea.mouseX
    property var mouseY: id_mouseArea.mouseY
    id: id_btnImg
    width: height  
    height: 30
    mipmap: true
    MouseArea
    {   
        id:id_mouseArea
        anchors.fill: parent
        onPressed: 
        {
            id_btnImg.scale=1.1
        }
        onCanceled: 
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
