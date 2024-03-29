import QtQuick
import QtQuick.Controls.Universal

Rectangle {
    width: parent.width
    height: theText.height
    clip: true //rich text does not wrap or elide
    color: "transparent"
    property alias text: theText.text
    Text {
        id: theText
        color: config.myPalette.windowText
        text: ""
        linkColor: isDark(config.myPalette.window) ? Qt.lighter("#0000FF") : "#0000FF"
        onLinkActivated: Qt.openUrlExternally(link)
        MouseArea {
            id: maLink
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.NoButton
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
        ToolTip {
            text: theText.hoveredLink
            visible: maLink.containsMouse
            delay: 1000
            timeout: 5000
        }
    }
}
