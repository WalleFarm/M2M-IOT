import QtQuick 2.7
import QtQuick.Controls 2.0

SwipeView {
    id:id_gwSwipeView
    parent: theModelGw01.takeModelParent()
    anchors.fill: parent
   
//         interactive: false//禁用手滑切换

    Gw01View
    {
        id:id_gw01View
        visible: id_gwSwipeView.currentIndex===0
        onSiqGotoNodeView:
        {
            id_gwSwipeView.currentIndex=1
        }
    }

    
     Node01View
     {
         id:id_node01View
        visible: id_gwSwipeView.currentIndex===1
        onSiqGotoGwView:
        {
            id_gwSwipeView.currentIndex=0
        }
     }
}


