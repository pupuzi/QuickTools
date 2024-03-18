import QtQuick 2.0
import QtQuick.Controls 2.12

CheckBox {
    id: root
    checked: true
    anchors.centerIn: parent
    spacing: 0
    font.pointSize: 24
    enabled: true

    indicator:Image {
          id: image
          height: 38
          width: 38
          anchors.verticalCenter: parent.verticalCenter
          source: root.checked ? "qrc:/images/checked.png" : "qrc:/images/unchecked.png"
    }
    //文本
    contentItem: Text {
        id: text
        text: root.text
        font: root.font
        opacity: enabled ? 1.0 : 0.3
        color: "black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
