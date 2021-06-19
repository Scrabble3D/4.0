import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Qt.labs.platform

RowLayout {
    property ListModel pickerColors:({})

    ComboBox {
        id: cbColors
        model: pickerColors
        delegate: Rectangle {
            implicitWidth: 180
            implicitHeight: 40
            Rectangle {
                width: parent.height * 0.75
                height: width
                radius: width * 0.5
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: width * 0.25
                color: itemColor
            }
            Text {
                leftPadding: parent.height * 1.25
                text: itemName
                anchors.verticalCenter: parent.verticalCenter
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    cbColors.currentIndex = index
                    rcBackground.color = itemColor
                    lbBackground.text = itemName
                    cbColors.popup.close()
                }
            }
        }

        background: Rectangle {
            implicitWidth: 180
            implicitHeight: 40
            Rectangle {
                id: rcBackground
                width: parent.height * 0.75
                height: width
                radius: width * 0.5
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: width * 0.25
            }
            Text {
                id: lbBackground
                leftPadding: parent.height * 1.25
                anchors.verticalCenter: parent.verticalCenter
            }
            layer.enabled: true
            layer.effect: DropShadow {
                transparentBorder: true
                color: "lightgrey"
                horizontalOffset: 2
                verticalOffset: 2
                radius: cbColors.hovered ? 15 : 5
            }
        }

        Component.onCompleted: {
            currentIndex: 1
            rcBackground.color = pickerColors.get(1).itemColor
            lbBackground.text = pickerColors.get(1).itemName
        }

    }

    Button {
        id: btnColors
        text: qsTr("...")
        onClicked: {
            colorDialog.open()
        }
    }

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        color: rcBackground.color
        onAccepted: {
            pickerColors.setProperty(cbColors.currentIndex, "itemColor", colorDialog.color)
            rcBackground.color = colorDialog.color
        }
        onRejected: {
            console.log("Canceled")
        }
    }

}

