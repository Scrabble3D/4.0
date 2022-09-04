import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform

RowLayout {
    property ListModel pickerColors:({})

    ComboBox {
        id: cbColors
        model: pickerColors
        Layout.preferredWidth: 200
        Layout.preferredHeight: 28

        delegate: Rectangle {
            height: 32
            width: parent.width
            color: myPalette.button
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
                color: myPalette.buttonText
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
                anchors.fill: parent
                color: myPalette.button
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
                    color: myPalette.buttonText
                    leftPadding: parent.height * 1.25
                    anchors.verticalCenter: parent.verticalCenter
                }

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

