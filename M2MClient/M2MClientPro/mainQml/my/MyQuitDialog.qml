import QtQuick 2.0
import "../base"

//退出登录警示对话框


BaseWarnDialog {
    
    titleText: "确定退出登录吗?"
    property var groupName: ""
    
    okText: "退出"
    okColor: "#F00000"
    onSiqCancelClicked: 
    {
        funClose()
    }
    
    
}
