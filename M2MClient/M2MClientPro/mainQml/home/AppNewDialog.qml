import QtQuick 2.0
import "../base"

//新建应用对话框


BaseEditDialog {
    
    titleText: "新建应用"
    onSiqCancelClicked: 
    {
        funClose() 
    }
     
    onSiqOkClicked: 
    {
        if(text)
        {
            theCenterMan.requestNewApp(text)
        }
         funClose()
    }
    
}
