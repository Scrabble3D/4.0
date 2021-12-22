import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

TableView {
    implicitHeight: parent.height
    implicitWidth: parent.width

    clip: true
    columnWidthProvider: function(column) {
        const colwidth = 55 //TODO: calculate based on col1 width / widthof(01.01.1970)
        return column == 0 ? colwidth : parent.width - colwidth;
    }
    ScrollBar.vertical: ScrollBar { }
    model: GamePlay.msgModel
    delegate: Rectangle {
        implicitWidth: parent.width
        implicitHeight: msgText.height + 2
        color: who > -1 ? Qt.lighter(config.playercolors.get(who).itemColor) : "white"

        function isDark(aColor) {
            var temp = Qt.darker(aColor, 1) //Force conversion to color QML type object
            var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
            return temp.a > 0 && a >= 0.3
        }

        Text {
            id: msgText
            text: display
            width: parent.width
            wrapMode: Text.Wrap
            color: isDark(parent.color) ? "white" : "black"
        }
    }
}
