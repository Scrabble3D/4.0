import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    property string shadowText
    property color shadowColor
    property int shadowPadding: 3
    property bool active: false
    property int shadowVertAlign: Text.AlignVCenter
    property int shadowHorzAlign: Text.AlignLeft
    property int shadowElide: Text.ElideNone
    property int shadowHeight //content height
    property int shadowWidth  //content width

    border.width: active ? 1 : 0
    border.color: "red"

    Text {
        verticalAlignment: shadowVertAlign
        horizontalAlignment: shadowHorzAlign
        width: parent.width
        height: parent.height
        leftPadding: shadowPadding + 1
        clip: true
        elide: shadowElide
        text: shadowText
        visible: shadowColor != "transparent"
        color: shadowColor
    }
    Text {
        verticalAlignment: shadowVertAlign
        horizontalAlignment: shadowHorzAlign
        width: parent.width
        height: parent.height
        leftPadding: shadowPadding
        clip: true
        elide: shadowElide
        text: shadowText
    }
    TextMetrics {
        id: metrics
        elide: shadowElide
        text: shadowText
        onTextChanged: {
            shadowHeight = tightBoundingRect.height
            shadowWidth = tightBoundingRect.width + shadowPadding + 1
        }
    }
}
