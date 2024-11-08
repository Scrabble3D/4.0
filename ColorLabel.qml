import QtQuick
import QtQuick.Templates as T

// needed for dark mode on Android
T.Label {
    property var bgcolor: "" // for lists with highlighting; must not be "transparent"

    function isDark(aColor) {
        var temp = Qt.darker(aColor, 1) //Force conversion to color QML type object
        var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
        return temp.a > 0 && a >= 0.3
    }

    color: enabled
           ? (bgcolor !== "")
             ? isDark(bgcolor)
               ? "white"
               : "dark"
             : palette.windowText
           : palette.mid
}
