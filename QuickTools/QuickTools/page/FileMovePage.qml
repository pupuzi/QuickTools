import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12

import UIObject 1.0
import "../module"

Rectangle {
    id: pageFileMove
    anchors.fill: parent

    property int offset: 10
    property int editWidth: 400
    property int editHeight: 24

    FileMoveObject{
        id: fileMngr

        onLogChanged:
        {
            txtLog.append(fileMngr.log)
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
            title: "选择源文件夹路径"
            selectExisting: true
            selectFolder: true
            onAccepted: {
                editSourcePath.text = dlgSourcePath.fileUrl.toString().substring(8,dlgSourcePath.fileUrl.length)
                fileMngr.setSourcePath(editSourcePath.text)
            }
        }

        onClicked: {
            dlgSourcePath.open()
        }
    }

    Text {
        id: txtDestPath
        anchors.left: txtSourcePath.left
        anchors.top: txtSourcePath.bottom
        anchors.topMargin: offset + editHeight / 2
        text: "源文件路径:"
    }

    UIFlatTextEdit{
        id: editDestPath
        anchors.left: txtDestPath.right
        anchors.leftMargin: offset
        anchors.top: txtDestPath.top
        anchors.topMargin: -4
        height: editHeight
        width: editWidth
        selectByMouse: true
        readOnly: true
    }

    UIFlatButton{
        id: btnDestPath
        anchors.left: editDestPath.right
        anchors.leftMargin: offset
        anchors.top: editDestPath.top
        width: editHeight * 2
        height: editHeight
        text: "选择"

        FileDialog{
            id: dlgDestPath
            title: "选择源文件夹路径"
            selectExisting: true
            selectFolder: true
            onAccepted: {
                editDestPath.text = dlgDestPath.fileUrl.toString().substring(8,dlgDestPath.fileUrl.length)
                fileMngr.setDestPath(editDestPath.text)
            }
        }

        onClicked: {
            dlgDestPath.open()
        }
    }

    Text {
        id: txtName
        anchors.left: txtDestPath.left
        anchors.top: txtDestPath.bottom
        anchors.topMargin: offset + editHeight / 2
        text: "文件关键字:"
    }

    UIFlatTextEdit{
        id: editName
        anchors.left: txtName.right
        anchors.leftMargin: offset
        anchors.top: txtName.top
        anchors.topMargin: -4
        height: editHeight
        width: editWidth / 2
        selectByMouse: true
    }

    UIFlatButton{
        id: btnCopy
        anchors.left: editName.right
        anchors.leftMargin: offset
        anchors.top: editName.top
        width: editHeight * 2
        height: editHeight
        text: "开始"

        onClicked: {
            fileMngr.setText(editName.text)
            fileMngr.copyFiles()
        }
    }



    UIRadiusRectangle{
            id: editLog
            anchors.left: txtName.left
            anchors.right: btnDestPath.right
            anchors.top: txtName.bottom
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
