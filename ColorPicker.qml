import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform //color dialog

RowLayout {
    property ListModel pickerColors:({})

    ColorComboBox {
        id: cbColors

        model: pickerColors
        Layout.preferredWidth: 200
        Layout.preferredHeight: 28

        delegate: Rectangle {
            height: 32
            width: parent.width
            color: config.myPalette.button
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
                color: config.myPalette.buttonText
                font: cbColors.font
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
            anchors.fill: parent
            Rectangle {
                anchors.fill: parent
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: !cbColors.down ? Qt.lighter(config.myPalette.button, 1.2) : config.myPalette.button
                    }
                    GradientStop {
                        position: 0.2
                        color: config.myPalette.button
                    }
                    GradientStop {
                        position: 0.8
                        color: !cbColors.down ? Qt.darker(config.myPalette.button, 1.2) : config.myPalette.button
                    }
                }
                border.width: cbColors.hovered ? 2 : 0
                border.color: config.myPalette.button

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
                    color: config.myPalette.buttonText
                    font: cbColors.font
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
        Layout.preferredHeight: 28
        Layout.preferredWidth: 28
        text: "..."
        onClicked: colorDialog.open()
    }

    ColorDialog {
        id: colorDialog
        title: qsTr("Please choose a color")
        color: rcColorDot.color
        onAccepted: {
            pickerColors.setProperty(cbColors.currentIndex, "itemColor", colorDialog.color)
            rcColorDot.color = colorDialog.color
        }
    }

}

