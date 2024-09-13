import QtQuick 2.0
import "../base"

//新建分组对话框


BaseEditDialog {
    
    titleText: "新建分组"
    onSiqCancelClicked: 
    {
        funClose() 
    }
     
    onSiqOkClicked: 
    {
        if(text)
        {
            if(text==="全部")
            {
                id_msgDialog.funOpen("不能使用\"全部\"作为组名!")
                return
            }
            theCenterMan.requestAddGroup(text)
        }
         funClose()
    }
    
}
