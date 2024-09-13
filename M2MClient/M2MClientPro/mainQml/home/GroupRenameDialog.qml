import QtQuick 2.0
import "../base"

//重命名分组对话框


BaseEditDialog {
    
    titleText: "重命名分组"
    property var oldName: ""
    onSiqCancelClicked: 
    {
        funClose()
    }
    
    
}
