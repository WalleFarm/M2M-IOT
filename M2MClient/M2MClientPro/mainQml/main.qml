import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    visible: true
    width: 380
    height: 750
    title: qsTr("端点物联")
    
    MainView
    {
        id:id_mainView
        anchors.fill: parent
        focus: true
        
        
    }
    onActiveFocusItemChanged: 
    {
        if(activeFocusItem)
        {
           
            var item_str=activeFocusItem.toString()
//             console.log("*********focus item=", item_str)
            if(item_str.indexOf("RootItem")>=0)
            {
                 funSetFocus()
            }
        }
        else
        {
            funSetFocus()
        }
    }
    function funSetFocus()
    {
        id_mainView.funSetFocus()
    }
}
