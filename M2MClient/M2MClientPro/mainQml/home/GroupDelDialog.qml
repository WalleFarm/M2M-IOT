import QtQuick 2.0
import "../base"

//删除警示对话框


BaseWarnDialog {
    
    titleText: "确认删除分组吗?"
    property var groupName: ""
    
    okText: "删除"
    okColor: "#F00000"
    onSiqCancelClicked: 
    {
        funClose()
    }
    
    
}
