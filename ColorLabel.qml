import QtQuick
import QtQuick.Controls

// needed for dark mode on Android
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
