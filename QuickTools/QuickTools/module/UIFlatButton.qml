import QtQuick 2.0
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0

Button {
    id: button

    property color clrForeground: "white"
    property color clrBackground: "#74c0fc"
    property int pixelSize: 12

    contentItem: Text {
        text: button.text
        color: button.clrForeground
        font.pixelSize: pixelSize
        font.family: "Arial"
        font.weight: Font.Thin
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 80
        implicitHeight: 35
        color: button.down ? Qt.darker(clrBackground, 1.2) : button.clrBackground
        radius: 3
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: button.down ? Qt.darker(clrBackground, 1.2) : button.clrBackground
            samples: 20
        }
    }
}
