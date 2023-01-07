import QtQuick
import QtQuick.Controls

SpinBox {
    id: control
    implicitWidth: 60 + 28
    implicitHeight: 28
    editable: true

    background: Rectangle {
        anchors.fill: parent
        border.width: 1
        border.color: palette.mid
        color: control.enabled ? "white" : "lightgrey"
    }

    contentItem: TextInput {
        z: 2
        text: control.textFromValue(control.value, control.locale)

        font: control.font
        color: control.enabled ? "black" : "darkgrey"
        selectionColor: config.myPalette.highlight
        selectedTextColor: config.myPalette.highlightedText

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        anchors.fill: parent //required for AlignVCenter on Android (Qt 6.2)
        anchors.rightMargin: up.width

        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly

    }

    up.indicator: Rectangle {
        id: up
        width: control.height / 2
        height: width
        x: control.width - width
        y: 0
        color: "transparent"

        Text {
            font.pointSize: parent.width < 6 ? 6 : parent.width - 6
            text: "▲"
            color: control.enabled ? "black" : "darkgrey"
//            anchors.centerIn: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            topPadding: 1
        }
    }
    down.indicator: Rectangle {
        id: down
        width: control.height / 2
        height: width
        x: control.width - width
        y: width
        color: "transparent"
        Text {
            font.pointSize: parent.width < 6 ? 6 : parent.width - 6
            color: control.enabled ? "black" : "darkgrey"
            text: "▼"
//            anchors.centerIn: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            bottomPadding: 1
        }
    }
}
