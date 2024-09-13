import QtQuick 2.0
import "../base"

//重命名设备对话框


BaseEditDialog {
    
    titleText: "设备重命名" 
    property var oldName: ""
    property var devSn: 0
    onSiqCancelClicked: 
    {
        funClose()
    }
    
    
}
