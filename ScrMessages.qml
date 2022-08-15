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
            text: display //TODO: messages: emoticons :-) = U+1F600
            width: delegateRect.width
            wrapMode: (model.column === 1)
               ? Text.WordWrap //wrap only the message
               : Text.NoWrap
            color: isDark(parent.color) //isDark() in mainwindow.qmlx
               ? "white"
               : "black"
        }
    }
    //FIXME: messages: contentheight signal not emitted in portrait mode
    onContentHeightChanged: {
        newMessage = !visible
     }
    //FIXME: messages: forecLayout() works but throws warnings while loading
    onWidthChanged: {
        forceLayout() //needed to ensure wordwrap on app resizing
    }
}
