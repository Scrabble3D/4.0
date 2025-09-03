import QtQuick
import QtQuick.Controls

RoundButton {
    implicitWidth: 50
    implicitHeight: 50

    display: AbstractButton.IconOnly
    icon.width: 32
    icon.height: 32

    property string tiptext: ""
    property string color: "darkgrey"
    property bool isHover: false
    property bool isActive: false
    property int progress: 0
    onProgressChanged: {
        if (progress < 1)
            progress = 100
        else
            background.requestPaint()
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            isHover = true
            background.requestPaint()
        }
        onExited: {
            isHover = false
            background.requestPaint()
        }
    }

    background: Canvas {
        id: background
        anchors.fill: parent

        onPaint: {
            var center  = Qt.vector2d(width / 2, height / 2)

            var context = getContext("2d");
            context.reset()

            context.beginPath()
            var perc = actionButton.progress/100
            var pi2 = Math.PI / 2
            context.arc(center.x, center.y, width / 2, pi2 - Math.PI * perc, pi2 + Math.PI * perc);
            context.fillStyle = isHover ? Qt.lighter(color, isActive ? 1.5 : 1.1) : color
            context.fill()
            context.closePath()
        }
    }

    ToolTip {
        text: tiptext
        visible: isActive && isHover
        delay: 1000
        timeout: 5000
    }
}
