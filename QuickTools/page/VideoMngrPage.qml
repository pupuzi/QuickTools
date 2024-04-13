import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12

import UIObject 1.0
import "../module"

Rectangle {
    id: pageVideoMngr
    anchors.fill: parent

    property int offset: 10
    property int editWidth: 400
    property int editHeight: 24

    QuickToolObject{
        id: quicktoolObject

        onLogChanged:
        {
            txtLog.append(quicktoolObject.log)
            if(1.0 - vscroll.size > 0)
            {
                vscroll.position = 1.0 - vscroll.size
            }
        }
    }

    Text {
        id: txtSourcePath
        anchors.left: parent.left
        anchors.leftMargin: offset
        anchors.top: parent.top
        anchors.topMargin: offset
        text: "源文件路径:"
    }

    UIFlatTextEdit{
        id: editSourcePath
        anchors.left: txtSourcePath.right
        anchors.leftMargin: offset
        anchors.top: txtSourcePath.top
        anchors.topMargin: -4
        height: editHeight
        width: editWidth
        selectByMouse: true
        readOnly: true
    }

    UIFlatButton{
        id: btnSourcePath
        anchors.left: editSourcePath.right
        anchors.leftMargin: offset
        anchors.top: editSourcePath.top
        width: editHeight * 2
        height: editHeight
        text: "选择"

        FileDialog{
            id: dlgSourcePath
            title: "选择源文件"
            selectExisting: true
            selectFolder: false
            onAccepted: {
                editSourcePath.text = dlgSourcePath.fileUrl.toString().substring(8,dlgSourcePath.fileUrl.length)
                quicktoolObject.setSourcePath(editSourcePath.text)
            }
        }

        onClicked: {
            dlgSourcePath.open()
        }
    }

    UIFlatButton{
        id: btnCompress
        anchors.left: editSourcePath.right
        anchors.leftMargin: offset
        anchors.top: btnSourcePath.bottom
        anchors.topMargin: 20
        width: editHeight * 2
        height: editHeight
        text: "压制"

        onClicked: {
            quicktoolObject.compressVideos();
        }
    }

    UIRadiusRectangle{
            id: editLog
            anchors.left: txtSourcePath.left
            anchors.right: btnCompress.right
            anchors.top: btnCompress.bottom
            anchors.topMargin: offset + editHeight / 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: offset
            borderColor: "#74c0fc"
            borderWidth: 2
            color: "white"
            cornersRadius: [5, 5, 5, 5]
            clip: true

            TextEdit{
                id: txtLog
                width: parent.width - offset + 6
                height: contentHeight
                y: -vscroll.position * txtLog.height + 5
                readOnly: true
                wrapMode: TextEdit.Wrap
                selectByMouse: true

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
                        txtLog.forceActiveFocus()
                    }
                }
            }

            ScrollBar{
                id: vscroll
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width: offset - 6
                hoverEnabled: true
                active: hovered || pressed
                orientation: Qt.Vertical
                size: editLog.height / txtLog.height
            }
        }
}
