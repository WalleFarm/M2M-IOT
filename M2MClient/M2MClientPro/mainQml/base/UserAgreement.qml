import QtQuick 2.0

//用户协议

BaseCheckBox01 {
    
    checkedImage: "qrc:/imagesRC/mainImages/check.png"
    checkedText: "已阅读并同意"
    
    width: height*4.5
    TextButton01{
        id:id_userText01
        textValue: "用户协议"
        textColor: "blue"
        
        anchors
        {
            verticalCenter:parent.verticalCenter
            left:parent.right
            leftMargin:2
        }
        onSiqClickedLeft: 
        {
            Qt.openUrlExternally("https://static.account.xiaomi.com/html/agreement/user/zh_CN.html")//示范网址,请勿商用
        }
    }
    TextButton01{
        id:id_userText02
        textValue: "隐私政策"
        textColor: "blue"
        
        anchors
        {
            verticalCenter:parent.verticalCenter
            left:id_userText01.left
            leftMargin:id_userText01.contentWidth+5
        }
        onSiqClickedLeft: 
        {
            Qt.openUrlExternally("https://privacy.mi.com/miaccount/zh_CN/")//示范网址,请勿商用
        }
    }
}
