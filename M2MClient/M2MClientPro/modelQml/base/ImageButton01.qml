import QtQuick 2.7

Image
{
    property var imgSrc: ""
    signal siqClickedLeft()
    id: id_btnImg
    width: 40  
    height: 30
    mipmap: true
    source: imgSrc
    MouseArea
    {
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
