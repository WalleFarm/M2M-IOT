import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0
import QtQuick.Particles 2.12
import QtQuick 2.14


Rectangle
{
    property var alarmLevel: 0
    
    id:id_onCircular
    Canvas {
        id:id_canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();

            var centreX = width / 2;
            var centreY = height / 2;
            var radius=width/2;
            
            // 画径向渐变色
            var gradient=ctx.createRadialGradient(centreX,centreY,0,centreX,centreY,radius);
            gradient.addColorStop(0, funTakeCenterColor());
            gradient.addColorStop(1, 'transparent');
            ctx.fillStyle=gradient;
            ctx.arc(centreX, centreY, width / 2, 0, Math.PI * 2, false);
            ctx.fill();
        }
    }

    function funTakeCenterColor()
    {
        var color="#0ee7cb"
        switch(alarmLevel)
        {
            case 0:color="#0ee7cb";break;
            case 1:color="#dd9220";break;
            case 2:color="#f65345";break;
               
        }
        return color
    }
    onAlarmLevelChanged: 
    {
        id_canvas.requestPaint()
    }
    
}
