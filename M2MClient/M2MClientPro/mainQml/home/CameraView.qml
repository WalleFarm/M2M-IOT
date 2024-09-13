import QtQuick 2.0
import QtMultimedia 5.12
import QtQuick 2.6
import QtQuick.Controls 2.5
//import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.12
import "../base"

Popup { 
    property var rowHeight: 30
    property var isWindows: Qt.platform.os==="windows" //是否为windows
    property var screenWidth:Screen.width
    property var screenHeight:Screen.height
    property var isCameraEnable: id_scanCamera.cameraState ==Camera.ActiveState
    id:id_popup
    visible: false
    width: parent.width*0.8
    height: width
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape// | Popup.CloseOnReleaseOutside
    anchors.centerIn: Overlay.overlay
    
    background: Rectangle
    {
        color:"transparent"
    }
    Rectangle//画面矩形
    {
        id:id_cameraRect
        width: isWindows ? 350 : screenWidth
        height: isWindows ? 550 : screenHeight
        color: "transparent"
//        color: Qt.rgba(0.8, 0.8, 0.8, 0.5)
        anchors.centerIn: parent

        Camera 
        {
            id:id_scanCamera
            captureMode: Camera.CaptureStillImage
            cameraState: Camera.UnloadedState
        }

        VideoOutput { 
            id: id_viewFinder
            
            width: parent.width*1.0
            height: parent.height
            anchors
            {
                horizontalCenter:parent.horizontalCenter
                verticalCenter:parent.verticalCenter
            }
            source: id_scanCamera
            autoOrientation: true
            fillMode: VideoOutput.PreserveAspectCrop
            MouseArea
            {
                anchors.fill: parent
                onClicked: //单机聚焦
                {
                    console.log("camera onClicked!")
                    if(isCameraEnable && id_scanCamera.focus.isFocusModeSupported(Camera.FocusAuto))  
                    {
                        console.log("support focus auto!")
                        id_scanCamera.searchAndLock()
                    }
                    else
                    {
                        console.log("do not support focus auto!  enable=", isCameraEnable)
                    }
                }
            }

        }
    
        ImageButton01 { //退出按钮
            id: id_quitImg           
            height: 40
            width: height
            mipmap: true
            anchors
            {
                bottom:parent.bottom
                bottomMargin:rowHeight*0.3
                horizontalCenter:parent.horizontalCenter
            }
            source: "qrc:/imagesRC/mainImages/home/quit.png"            
            onSiqClickedLeft: 
            {
                console.log("camera quit***")
                funCloseView()
            }
        }
        
        
        Rectangle//闪烁横杠
        {
            id:id_lineRect
            height: 2
            width: id_zoneRect.width*0.8
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(230/255, 8/255, 5/255, 0.5) }
                GradientStop { position: 0.5; color: Qt.rgba(230/255, 8/255, 5/255, 1.0) }
                GradientStop { position: 1.0; color: Qt.rgba(230/255, 8/255, 5/255, 0.5) }
            }
            anchors
            {
                verticalCenter:parent.verticalCenter      
                horizontalCenter:parent.horizontalCenter
            }
        }
        Rectangle
        {
            id:id_zoneRect
            width: 200
            height: 200
            border.width: 2
            border.color: "green"
            color: "transparent"
            anchors.centerIn: parent
        }
        Timer
        {
            interval: 500
            running: isCameraEnable
            repeat: true
            onTriggered: 
            {
                id_lineRect.visible=!id_lineRect.visible
            }
        }
        Timer
        {
            interval: 500
            running: isCameraEnable && id_popup.visible
            repeat: true
            onTriggered: 
            {
                if(id_scanCamera.imageCapture.ready)
                {
//                    console.log("capture***") 
                    theMainInterface.setQrImage(id_viewFinder)
                }
            }
        }
    }

    onVisibleChanged: 
    {
        console.log("onVisibleChanged=", visible)
        if(visible===false)
        {
            funCloseView()
            
        }
    }
    
    Connections
    {
        target: theMainInterface
        onSiqDecodeSuccess:
        {
            console.log("success scan=", result)
            id_devAddDialog.funOpen(result)
            funCloseView()//扫码成功,关闭
        }
    }
    
    function funOpenView()
    {
        id_popup.open()
        id_scanCamera.start()
        for (var i in Camera.availableDevices) {
            var device = Camera.availableDevices[i];
            console.log(" Camera Device:", device.deviceId, device.position)
        }
    }    
    function funCloseView()
    {
        id_scanCamera.stop()
        id_popup.close()
    }
}


