import QtQuick 2.0
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0

TextField {
    id: textEdit

    property color clrBase: "#74c0fc"
    //property bool multiLines: false

    font.family: "Arial"
    font.pixelSize: 12
    font.weight: Font.Thin
    antialiasing: true
    clip: true

    background: Rectangle{
        implicitWidth: 200
        implicitHeight: 40
        radius: 3
        color: textEdit.enabled ? "transparent" : "lightgray"
        border.color: clrBase
        border.width: 1
        opacity: textEdit.enabled ? 1 : 0.7
        layer.enabled: textEdit.hovered
        layer.effect: DropShadow {
            transparentBorder: true
            color: textEdit.focus ? Qt.darker(clrBase, 1.2) : clrBase
            samples: 10
        }
    }

    /*
    ScrollBar{
        id: vscroll
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: 10
        hoverEnabled: true
        active: hovered || pressed
        orientation: Qt.Vertical
        size: textEdit.height / textEdit.contentHeight
    }

    MouseArea{
        anchors.fill: parent

        onWheel: {
            if(wheel.angleDelta.y > 0)
            {
                vscroll.decrease()
            }
            else
            {
                vscroll.increase()
            }
        }

        onClicked: {
            textEdit.forceActiveFocus()
        }
    }
    )*/
}
