import QtQuick 2.7
import QtQuick.Controls 2.0
import "base"
import "home"
import "my"

//控制中心的切换页面

Page {
    id:id_centerPage

    footer: Rectangle
    {
        height:50
        width:parent.width
        FootTabBar 
        {
             id: id_tabBar
            anchors.fill: parent
            onCurrentIndexChanged: 
            {
                id_centerSwipeView.currentIndex=currentIndex
            }
        }
    }
    
    SwipeView
    {
        id: id_centerSwipeView
        anchors.fill: parent
        interactive: false//禁用手滑切换
        currentIndex: id_tabBar.currentIndex
        HomeView
        {
            visible: id_centerSwipeView.currentIndex==0
        }
        Rectangle
        {
            id:id_modelView
            visible: id_centerSwipeView.currentIndex==1
            gradient: Gradient 
            {
                GradientStop { position: 0.0; color: "#CFD5E6" }
                GradientStop { position: 1.0; color: "#F5F6F8" }
            }
        }   
        Rectangle
        {
            visible: id_centerSwipeView.currentIndex==2
            gradient: Gradient 
            {
                GradientStop { position: 0.0; color: "#CFD5E6" }
                GradientStop { position: 1.0; color: "#F5F6F8" }
            }
        }   
        MyView
        {
            visible: id_centerSwipeView.currentIndex==3
        }   
    }
    
    Component.onCompleted: 
    {
        theCenterMan.setModelParent(id_modelView)
    }
    
    Connections
    {
        target: theCenterMan
        onSiqSetCenterCurrView:
        {
            console.log("CenterView_name=", view_name)
            if(view_name=="center-home")
            {
                id_tabBar.currentIndex=0
            }
            else if(view_name=="center-dev")
            {
                id_tabBar.currentIndex=1
            }
            else if(view_name=="center-msg")
            {
                id_tabBar.currentIndex=2
            }
            else if(view_name=="center-my")
            {
                id_tabBar.currentIndex=3
            }
        }
    }
    
}
