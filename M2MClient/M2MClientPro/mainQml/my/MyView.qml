import QtQuick 2.7
import QtQuick.Controls 2.0

import "../base"

SwipeView {
    id:id_mySwipeView
    property var myLoginState: 0
    interactive: false//禁用手滑切换
    currentIndex: 0
    
    MyQuitDialog
    {
        id:id_myQuitDialog
        onSiqOkClicked: 
        {
            myLoginState=0
            id_myQuitDialog.funClose()
            theAccountMan.setViewByLoginState(0)
        }
    }
    
    Rectangle{   //当前页面
        id:id_myMainView
        color: "#F5F6F8"
        MyHeadView  //头部
        {
            id:id_myHeadView
            loginState: myLoginState
            anchors
            {
                top:parent.top
            }
        }
        MyMidView //中间 
        {
            id:id_myMidView
            anchors
            {
                top:id_myHeadView.bottom
                topMargin:-30
                horizontalCenter:parent.horizontalCenter
            }
            
        }
        MyBottomView //底部
        {
            id:id_myBottomView
            anchors
            {
                top:id_myMidView.bottom
                topMargin:30
                horizontalCenter:parent.horizontalCenter
                bottom:parent.bottom
                bottomMargin:10
            }
            onSigUnionClicked: 
            {
                if(myLoginState===0)
                {
                    if(index<3)
                    {
                        theAccountMan.setViewByLoginState(0)
                    }
                    return
                }
                id_mySwipeView.currentIndex=index+1
//                switch(index)
//                {
//                    case 0:id_mySwipeView.currentIndex=1;break
                    
//                }
            }
        }
        
    }
    
    ChangePwdView
    {
        id:id_changePwdView
        visible: id_mySwipeView.currentIndex==1
        onSiqGoBackLevel0: 
        {
            id_mySwipeView.currentIndex=0
        }
    }
    
    Connections
    {
        target: theAccountMan
        onSiqUpdateLoginInfo:
        {
            console.log("***login_state=", login_state)
            myLoginState=login_state
            id_myHeadView.accountText=account
            id_myHeadView.levelValue=auth
            id_changePwdView.accountName=account
        }
    }
    
}
