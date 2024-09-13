import QtQuick 2.0
import QtQuick.Controls 2.0
Rectangle {
//    property var sliderColor: "#0ee7cb"
    property var currPostion: 0
    property var alarmLevel: 0
    property var switchState: 0
    height: 100
    width: parent.width*0.96
    
    Label{ //风速调节
        id:id_speedLabel
        height: 30
        width: 80
        anchors
        {
            left:parent.left
            leftMargin:10
            top:parent.top
            topMargin:5
        }
        
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
        font.pointSize: height*0.5
        font.family:Qt.platform.os === "windows" ? "宋体" : "黑体"
        color: "black"
        text:"风速调节"
    }
    Label{ //适用面积
        id:id_areaLabel
        height: 30
        width: 120
        anchors
        {
            verticalCenter:id_speedLabel.verticalCenter
            left:id_speedLabel.right
        }
        
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pointSize: height*0.5
        font.family:Qt.platform.os === "windows" ? "宋体" : "仿宋"
        color: "#B0B0B0"
        text:funTakeAreaText()
    }
    
    Slider {
        id:id_speedSlider
        width: parent.width*0.9
        height: parent.height*0.45
        anchors
        {
            horizontalCenter:parent.horizontalCenter
            bottom:parent.bottom
            bottomMargin:10
        }
        enabled: switchState
        from: 0  // 滑动条的起始值
       value: 50 // 滑动条的当前值
       to: 100   // 滑动条的结束值
       stepSize: 1 // 滑动条的步长
       background: Rectangle{  //背景
           id:id_backRect
            height: id_speedSlider.height
            width: id_speedSlider.width//+height
            
            radius: height/2
            color: "#F0F0F0"
            anchors.centerIn: id_speedSlider
            Rectangle {//左边填充
                height: id_backRect.height
                width: id_speedSlider.visualPosition * (parent.width-parent.height)+height
                anchors
                {
                    left:parent.left
                    verticalCenter:parent.verticalCenter
                }
                color: funTakeSliderColor()
                radius: height/2
            }
       }
       
       handle: Rectangle//滑块
       {
            height:id_backRect.height*0.85
            width:height
            radius:height/2
            x:id_speedSlider.height/2+id_speedSlider.visualPosition * (id_speedSlider.width-id_speedSlider.height)-height/2
            anchors.verticalCenter:parent.verticalCenter
       }
       
       onMoved: 
       {
           var det_pos=id_speedSlider.visualPosition-currPostion
           if(Math.abs(det_pos) >0.2)
           {
               theModelAp01.setFanSpeed(id_speedSlider.visualPosition)
                currPostion=id_speedSlider.visualPosition    
           }

       }
       onPressedChanged: 
       {
            if(!pressed)
            {
                theModelAp01.setFanSpeed(id_speedSlider.visualPosition)
                console.log("********")
            }
       }
       
       
    }
    
    Connections
    {
        target: theModelAp01
        onSiqUpdateAlarmLevel:
        {
            alarmLevel=alarm_level
        }
        onSiqUpdateSwitchState:
        {
            switchState=state
        }
    }
    
    function funTakeSliderColor()
    {
        var color="#D0D0D0"
        if(switchState>0)
        {
            switch(alarmLevel)
            {
                case 0:color="#0ee7cb";break;
                case 1:color="#dd9220";break;
                case 2:color="#f65345";break;
                   
            }
        }

        return color
    }
    
    function funTakeAreaText()
    {
        var start_area=3+ Math.floor(id_speedSlider.visualPosition*20) 
        var stop_area=start_area+3+Math.floor(id_speedSlider.visualPosition*15)
        
        var text=" | 适用面积"+start_area+"~"+stop_area+"m2"
        return text
    }
    
    
}
