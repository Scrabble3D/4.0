import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

TableView {
    Layout.fillHeight: true
    Layout.fillWidth: true

    property bool newMessage: false

    TextMetrics {
        id: textMetrics
        text: "01.01.1970" //first column width
    }

    columnWidthProvider: function(column) {
        const colwidth = textMetrics.width
        return (column === 0)
                ? colwidth
                : width - colwidth;
    }

    ScrollBar.vertical: ScrollBar { }
    model: GamePlay.msgModel

    delegate: Rectangle {
        id: delegateRect
        implicitHeight: msgText.height + 2
        color: who > -1
               ? Qt.lighter(config.playercolors.get(who).itemColor)
               : config.myPalette.window
        Text {
            id: msgText
            leftPadding: 4
            rightPadding: 4
            text: display // FEATURE: messages: emoticons :-) = U+1F600
            onLinkActivated: (link)=> Qt.openUrlExternally(link)
            width: delegateRect.width
            wrapMode: (model.column === 1)
                       ? Text.WordWrap //wrap only the message
                       : Text.NoWrap
            color: isDark(parent.color) //isDark() in mainwindow.qmlx
                       ? "white"
                       : "black"
            linkColor: isDark(config.myPalette.window)
                       ? Qt.lighter("#0000FF")
                       : "#0000FF"
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
    onContentHeightChanged: newMessage = true //show indicator on new messages
}
