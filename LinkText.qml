import QtQuick
import QtQuick.Controls.Universal

Text {
    text: ""
    color: config.myPalette.windowText
    linkColor: isDark(config.myPalette.window) ? Qt.lighter("#0000FF") : "#0000FF"
    onLinkActivated: Qt.openUrlExternally(link)
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

}
