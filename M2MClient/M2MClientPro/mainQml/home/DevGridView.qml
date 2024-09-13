import QtQuick 2.7
import QtQuick.Controls 2.0
import "../base"

Rectangle
{
    property var groupName: ""

    id:id_rootRect
    height: parent.height
    width: parent.width-20
    color: "transparent"
    anchors.horizontalCenter: parent.horizontalCenter
    DevDelDialog//设备删除对话框
    {
        id:id_devDelDialog
        onSiqOkClicked: 
        {
            var dev_list=[]
            var ptr=0
            for(var i=0; i<id_cellModel.count; i++)
            {
                if(id_cellModel.get(i).select_state)//选中
                {
                    dev_list[ptr++]=id_cellModel.get(i).dev_sn
                }
            }
            theCenterMan.requestDelDevice(dev_list)
            funClose()
        }
    }

    DevRenameDialog//设备重命名对话框
    {
        id:id_devRenameDialog
        
        onSiqOkClicked: 
        {
            theCenterMan.requestRenameDevice(devSn, text)
            funClose()
        }
        
    } 
    
    GridView {
        id:id_gridView
        property int dragItemIndex: -1
        property var dragActive: false
        property int selectCnts: 0
        property var sortFlag: false
        anchors.fill: parent
        clip: true
        cacheBuffer: 10000
        implicitWidth: 150
        implicitHeight: 150
        cellWidth: width*0.5
        cellHeight: cellWidth*0.8
        
        move:Transition {
            NumberAnimation { properties: "x,y"; duration: 200 }
        }
        moveDisplaced:Transition {
            NumberAnimation { properties: "x,y"; duration: 200 }
        }
        
        model: ListModel{
            id:id_cellModel
        }
        delegate: Rectangle{//单元格矩形
            id:id_cellRect
//            property var selectState: false
            color: "transparent"
//            border.color: "blue"
//            border.width: 1
            width: id_gridView.cellWidth
            height: id_gridView.cellHeight
            
            Rectangle //拖动矩形
            {
                id:id_dragRect
                radius: 12
                width: id_cellRect.width-15
                height:id_cellRect.height-15
                anchors.centerIn:id_cellRect
                color: id_moveMouseArea.drag.active ? "transparent" : "white"
                
                onParentChanged: 
                {
                    if(parent!=null)
                    {
                        theCenterMan.showSimpleView(dev_sn, id_dragRect)
                    }         
                } 
                
                Rectangle{  //勾选框
                    id:id_selectRect
                    width: 24
                    height: width
                    radius: width/2
                    border.width: 1
                    border.color: "#D0D0D0"
                    visible: id_gridView.dragActive
                    anchors   
                    {
//                        top:parent.top
//                        topMargin:15
                        bottom:parent.bottom
                        bottomMargin:15
                        right:parent.right
                        rightMargin:15
                    }
                    Image {
                        id: id_selectImage
                        anchors.fill: parent
                        visible: select_state
                        mipmap: true
                        source: "qrc:/imagesRC/mainImages/check.png"
                    }
                }
                
                MouseArea
                {
                    id:id_moveMouseArea
                    anchors.fill: parent
//                    drag.target: id_dragRect
                    drag.axis: Drag.YAxis | Drag.XAxis
                    drag.onActiveChanged: {
                        if (id_moveMouseArea.drag.active) 
                        {
                            id_gridView.dragItemIndex = index;
                                console.log("drag index=", index)
                        }
                        id_dragRect.Drag.drop();
                    }
                    onClicked: 
                    {
                        if(id_gridView.dragActive)
                        {
                            select_state=!select_state
                            if(select_state)
                                id_gridView.selectCnts++
                            else
                                id_gridView.selectCnts--
//                            console.log("selectCnts=", id_gridView.selectCnts)
                        }
                        else
                        {
                            console.log("clicked dev_sn=",dev_sn)
                            theCenterMan.showModelView(dev_sn)
                        }
                    }
                    onPressAndHold: 
                    {
                        id_gridView.dragActive=true
                        select_state=true
                        id_gridView.selectCnts=1
                    }
                    onReleased: 
                    {
//                        drag.target=null
//                        id_gridView.dragActive=false
//                        id_dropAnimation.stop()
                    }
                }
                states: [
                    State {
                        when: id_dragRect.Drag.active
                        ParentChange {
                            target: id_dragRect
                            parent: id_rootRect
                        }
    
                        AnchorChanges {
                            target: id_dragRect
                            anchors.horizontalCenter: undefined
                            anchors.verticalCenter: undefined
                        }
                    }
                ]
                Drag.active: id_moveMouseArea.drag.active
                Drag.hotSpot.x: id_dragRect.width / 2
                Drag.hotSpot.y: id_dragRect.height / 2
                
            }
            DropArea {  //拖拽
            id: dropArea
            anchors.fill: parent
            onDropped:{
                console.log("onDropped")
                var other_index = id_gridView.indexAt(id_moveMouseArea.mouseX + id_cellRect.x, id_moveMouseArea.mouseY + id_cellRect.y);
                console.log("other_index:",other_index,"id_gridView.dragItemIndex:",id_gridView.dragItemIndex);
                if(other_index!==id_gridView.dragItemIndex && other_index>=0)
                {
                    id_cellModel.move(id_gridView.dragItemIndex,other_index, 1);
                    id_gridView.sortFlag=true//有排序动作
                }
            }
            
            Rectangle {
                    id: dropRectangle
    
                    anchors.centerIn:parent
                    width: id_dragRect.width
                    height: id_dragRect.height
                    color: "transparent"
                    radius: 12
                    states: [
                        State {
                            when: dropArea.containsDrag
                            PropertyChanges {
                                 target: dropRectangle
                                 color: "lightsteelblue"
                                 opacity:0.3
                            }
                        }
                    ]
                }//end Rectangle
    
            }//end drop
            
            PropertyAnimation { //抖动
                id:id_dropAnimation
                target: id_dragRect
                properties: "rotation"
                from:-1.5
                to:1.5
                duration: 300
                easing.type: Easing.InOutExpo
                loops: 300 //循环次数
                onStopped: 
                {
                    target["rotation"] = 0 //显示归位
                }
            }
            Timer{
                interval: Math.random()*500; running: true; repeat: true
                onTriggered: 
                {
                    if(id_gridView.dragActive)
                    {
                        if(id_dropAnimation.stopped)
                        {
                            id_dropAnimation.start()
                            id_moveMouseArea.drag.target=id_dragRect         
                        }
                    }
                    else
                    {
                        if(id_dropAnimation.started)
                        {
                            id_dropAnimation.stop()
                            id_moveMouseArea.drag.target=null
                            select_state=false
                            id_gridView.selectCnts=0
                        }
                    }
                }
            }
    
        }//end delegate
        
    }//end GridView
    
    Rectangle  //设置栏
    {
        id:id_setRect
        
        width: parent.parent.width
        height: 60
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            bottom:id_gridView.bottom
        }
        MouseArea{
            anchors.fill: parent //接收鼠标事件,避免选择背后的单元格
        }
        color: "#606060"
//        opacity: 0.5
        visible: id_gridView.dragActive
        
        Row
        {
            height: id_setRect.height
            width: id_setRect.width*0.9
            anchors
            {
                top:parent.top
                topMargin:5
                horizontalCenter:id_setRect.horizontalCenter
            }
            spacing: (width-30*4)/3
            Repeater
            {
                model: ListModel{
                    id:id_setModel
                }
                Rectangle{
                    property var maskFlag: index===0 && id_gridView.selectCnts!==1
                    height: 30
                    width: height
                    radius: width/2
                    color:  maskFlag ? "#808080" : "white"
                    ImageButton01 {
                        anchors.centerIn: parent
                        width: parent.width*0.7
                        height: width
                        mipmap: true 
                        source: img_src
                        onSiqClickedLeft: 
                        {
//                            console.log("clicked index=", index)
                            switch(index)
                            {
                                case 0: //修改名称
                                    if(id_gridView.selectCnts===1)
                                    {
                                        for(var i=0; i<id_cellModel.count; i++)
                                        {
                                            if(id_cellModel.get(i).select_state)//选中
                                            {
                                                id_devRenameDialog.devSn=id_cellModel.get(i).dev_sn
                                                id_devRenameDialog.oldName=theCenterMan.takeWorkDeviceName(id_devRenameDialog.devSn)//旧名称
                                                id_devRenameDialog.funOpen(id_devRenameDialog.oldName)
                                                break
                                            }
                                        }         
                                    }
                                    break
                                case 1: //移动设备
                                    id_devMoveDialog.open()
                                    var dev_list=[]
                                    var ptr=0
                                    for(i=0; i<id_cellModel.count; i++)
                                    {
                                        if(id_cellModel.get(i).select_state)//选中
                                        {
                                            dev_list[ptr++]=id_cellModel.get(i).dev_sn
                                        }
                                    }
                                    id_devMoveDialog.srcGroupName=groupName
                                    id_devMoveDialog.moveDevList=dev_list
                                    break
                                case 2: //删除设备
                                    id_devDelDialog.funOpen()
                                    break                                    
                                case 3:  //完成
                                    id_gridView.dragActive=false;
                                    if(id_gridView.sortFlag==true)//有排序动作
                                    {
                                        id_gridView.sortFlag=false
                                        dev_list=[]
                                        for(i=0; i<id_cellModel.count; i++)
                                        {
                                            dev_list[i]=id_cellModel.get(i).dev_sn
                                        }
                                        theCenterMan.requestSortDevice(groupName, dev_list)//排序
                                    }
                                    break
                            }
                        }
                    }
                    Text{
                        height: 25
                        anchors
                        {
                            horizontalCenter:parent.horizontalCenter
                            top:parent.bottom
                            topMargin:3
                        }
                        text: name
                        font.pointSize: 10
                        font.family: "宋体"
                        color:  maskFlag ? "#808080" : "white"
                    }
                }

            }
        }
        

        
    }
    
    
    Connections
    {
        target: theCenterMan
        onSiqAddDevice2Group:
        {
            if(group_name===groupName)
            {
                id_cellModel.append({"dev_sn":dev_sn, "select_state":false})
            }
        }
        onSiqDelDevice:
        {
            for(var i=0; i<id_cellModel.count; i++)
            {
                if(dev_sn===id_cellModel.get(i).dev_sn)
                {
                    id_cellModel.remove(i)
                    break
                }
            }
            if(flag>0)  //删除完成
            {
                var dev_list=[]
                var ptr=0
                for(i=0; i<id_cellModel.count; i++)
                {
                    dev_list[ptr++]=id_cellModel.get(i).dev_sn
                }
                theCenterMan.requestSortDevice(groupName, dev_list)//重新排序
            }
        }
        onSiqClearDevice:
        {
            if(group_name===groupName)
            {
                id_cellModel.clear()
            }
        }
    }
    
    Component.onCompleted: 
    {

        var img_src="qrc:/imagesRC/mainImages/home/rename.png"
        var name="修改名称"
        id_setModel.append({"img_src":img_src, "name":name})
        img_src="qrc:/imagesRC/mainImages/home/move.png"
        name="移动设备"
        id_setModel.append({"img_src":img_src, "name":name})  
        img_src="qrc:/imagesRC/mainImages/home/del02.png"
        name="删除设备"
        id_setModel.append({"img_src":img_src, "name":name})  
        img_src="qrc:/imagesRC/mainImages/home/finish.png"
        name="完成"
        id_setModel.append({"img_src":img_src, "name":name})  
        
    }
    
}
