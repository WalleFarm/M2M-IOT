import QtQuick 2.0
import "../base"

//删除警示对话框


BaseWarnDialog {
    
    titleText: "确认删除设备吗?\n如果只想把设备移出本组,\n请将设备移动到\"全部\"分组."
    property var groupName: ""
    
    okText: "删除"
    okColor: "#F00000"
    onSiqCancelClicked: 
    {
        funClose()
    }
    
    
}
