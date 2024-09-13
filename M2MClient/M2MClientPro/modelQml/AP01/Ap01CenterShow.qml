import QtQuick 2.0
import QtQuick.Controls 1.4



Rectangle {
    property var textColor: id_centerCircular.visible ? "white" : "black"
    property var switchState: 0
    id:id_rootRect
    width: 300
    height: 300
//    anchors.fill: parent
    color:"transparent"
    Rectangle  //关闭后 黑色圆圈
    {
        id:id_centerCircular //ID  中心圆
        visible: !switchState
        width:parent.width*0.7
        height: width
        radius: width/2
        color: "#062230"
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            top:parent.top
            topMargin:height*0.2
        }
    }
    
    Ap01ColorView//开启后彩色圆圈
    {
        id:id_colorView
        visible: switchState
        width: id_centerCircular.width
        height: id_centerCircular.height
        radius: id_centerCircular.radius
        anchors.centerIn: id_centerCircular
    }
    
    Repeater  //泡泡
    {
        model: ListModel{
            id:id_listModel
        }

        Rectangle{
            id:id_bubbles
            visible: switchState
            width: 8
            height: width
            radius: width/2
            color: id_colorView.funTakeCenterColor()
            x:0
            y:0
            Timer{
                interval: 50; running: switchState; repeat: true
                onTriggered: 
                {
                    var c_x=id_rootRect.width/2
                    var c_y=id_rootRect.height/2//矩形框的中心点
                    var det_x=c_x-id_bubbles.x
                    var det_y=c_y-id_bubbles.y
                    var det_val=Math.sqrt(det_x*det_x+det_y*det_y)//与中心点的距离
                    if(x==0 || y==0 || det_val<20)//消失,重新出发
                    {
                        var rand1=Math.random()
                        var rand2=Math.random()
                        var rand3=Math.random()//获取3个随机数
                        id_bubbles.width=rand3*6+4  //泡泡直径
                        id_bubbles.x=rand1*id_rootRect.width
                        id_bubbles.y=rand2*id_rootRect.height//出发位置
                    }
                    else
                    {
                        //向中心移动
                        if(id_bubbles.x<c_x-5)id_bubbles.x+=1
                        else if(id_bubbles.x>c_x+5)id_bubbles.x-=1
                        
                        if(id_bubbles.y<c_y-5)id_bubbles.y+=1
                        else if(id_bubbles.y>c_y+5)id_bubbles.y-=1
                    }
                }
            }
        }
    }

    
    Label{ //空气质量标题
        id:id_titleLabel
        height: 30
        width: parent.width
        anchors
        {
            bottom:id_airLevelLabel.top
            bottomMargin:10
            horizontalCenter:id_centerCircular.horizontalCenter
        }
        
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: height*0.45
        font.family:"黑体"
        color: textColor
        text:"室内综合空气质量"
    }
    
    Label{ //空气质量文本
        id:id_airLevelLabel
        height: 60
        width: parent.width
        anchors
        {
            verticalCenter:id_centerCircular.verticalCenter
            horizontalCenter:id_centerCircular.horizontalCenter
        }
        
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: height*0.8
        font.family:"黑体"
        color: textColor
        text:"空气优"
    }
    
    Label{ // 温湿度文本
        id:id_thLabel
        height: 30
        width: parent.width
        anchors
        {
            top:id_airLevelLabel.bottom
            topMargin:10
            horizontalCenter:id_centerCircular.horizontalCenter
        }
        
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: height*0.45
        font.family:"黑体"
        color: textColor
        text:"温度20℃ | 湿度52%"
    }
    
    Label{ // PM2.5标签
        id:id_pm25Label
        height: 30
        width: parent.width
        anchors
        {
            bottom:parent.bottom
            bottomMargin:30
            horizontalCenter:parent.horizontalCenter
        }
        
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: height*0.4
        font.family:"仿宋"
        color: "#B0B0B0"
        text:"PM2.5(ug/m3)"
    }
    Label{ // PM2.5数值
        id:id_pm25Value
        height: 30
        width: parent.width
        anchors
        {
            bottom:id_pm25Label.top
            horizontalCenter:parent.horizontalCenter
        }
        
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 25
        font.family:"宋体"
        color: "black"
        text:"013"
    }
    

    
    Component.onCompleted: 
    {
        for(var i=0; i<30; i++)
        {
            id_listModel.append({"index":i})
        }
        
        funUpdateThText(21, 53)
    }
    
    Connections
    {
        target: theModelAp01
        onSiqUpdateAlarmLevel:
        {
            id_colorView.alarmLevel=alarm_level
            switch(alarm_level)
            {
                case 0:id_airLevelLabel.text="空气优";break;
                case 1:id_airLevelLabel.text="空气良";break;
                case 2:id_airLevelLabel.text="空气差";break;
            }
        }
        onSiqUpdateSwitchState:
        {
            switchState=state
        }
    }
    
    function funUpdatePm25Text(pm25)
    {
        id_pm25Value.text=pm25
        
    }
    
    function funUpdateThText(temp, humi)
    {
        id_thLabel.text="温度"+temp+"℃ | 湿度"+humi+"%"
    }
    
    
}


















