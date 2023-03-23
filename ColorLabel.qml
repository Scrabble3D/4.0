import QtQuick
import QtQuick.Controls

Label {
    property var bgcolor: "" // for lists with highlighting; must not be "transparent"

    color: enabled
           ? (bgcolor !== "")
             ? isDark(bgcolor)
               ? "white"
               : "dark"
             : config.myPalette.windowText
           : config.myPalette.mid
}
