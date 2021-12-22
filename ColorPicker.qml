import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform
import Qt5Compat.GraphicalEffects

RowLayout {
    property ListModel pickerColors:({})

    ComboBox {
        id: cbColors
        model: pickerColors
        Layout.preferredWidth: 200

        delegate: Rectangle {
            height: 32
            width: parent.width
            Rectangle {
                id: delegateColorDot
                width: parent.height * 0.75
                height: width
                radius: width * 0.5
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: width * 0.25
                color: itemColor
            }
            Text {
                id: delegateColorName
                leftPadding: parent.height * 1.25
                text: itemName
                anchors.verticalCenter: parent.verticalCenter
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    cbColors.currentIndex = index
                    rcColorDot.color = itemColor
                    lbColorName.text = itemName
                    cbColors.popup.close()
                }
            }
        }

        background: Rectangle {
            id: rcBackground
            Rectangle {
                id: rcColorDot
                width: parent.height * 0.75
                height: width
                radius: width * 0.5
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: width * 0.25
            }
            Text {
                id: lbColorName
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
            rcColorDot.color = pickerColors.get(1).itemColor
            lbColorName.text = pickerColors.get(1).itemName
        }

    }

    Button {
        id: btnColors
        text: "..."
        onClicked: colorDialog.open()
    }

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        color: rcColorDot.color
        onAccepted: {
            pickerColors.setProperty(cbColors.currentIndex, "itemColor", colorDialog.color)
            rcColorDot.color = colorDialog.color
        }
    }

}

