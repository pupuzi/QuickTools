import QtQuick 2.12
import QtQuick.Controls 2.0
import QtQuick.Window 2.12
import QtGraphicalEffects 1.0

import "./module"
import "./page"

Window {
    id: window
    visible: true
    width: 640
    height: 480
    flags: "FramelessWindowHint"
    color: "#00000000"


    //窗口拖动
    property point mousePos: "0, 0"
    MouseArea{
        id: title
        anchors.left: parent.left
        width: 60
        height: parent.height

        onPressed: {
            mousePos = Qt.point(mouse.x, mouse.y)
        }

        onPositionChanged: {
            if(pressed)
            {
                var deltaPos = Qt.point(mouse.x - mousePos.x, mouse.y - mousePos.y)
                window.x += deltaPos.x
                window.y += deltaPos.y
            }
        }
    }

    //主窗口
    UIRadiusRectangle{
        id: framewindow
        color: "#f0f0f0"
        cornersRadius: [5, 5, 5, 5]
        anchors.fill: parent
        anchors.margins: 10

        UIRadiusRectangle{
            id: mainwindow
            color: "white"
            anchors.left: parent.left
            anchors.leftMargin: 60
            width: parent.width - 60
            height: parent.height
            cornersRadius: [0, 5, 5, 0]

            Loader{
                id: pageLoader
                anchors.fill: parent
            }
        }

        Button{
            id: btnFileMove
            anchors.top: parent.top
            anchors.topMargin: 60
            anchors.left: parent.left
            anchors.leftMargin: 10
            width: 40
            height: 40

            onClicked: {
                pageLoader.source = "qrc:/page/FileMovePage.qml"
            }
        }
    }

    //阴影
    DropShadow {
          anchors.fill: framewindow
          horizontalOffset: 1
          verticalOffset: 1
          radius: 5
          samples: 5
          source: framewindow
          color: "gray"
          Behavior on radius { PropertyAnimation { duration: 100 } }
        }
}
