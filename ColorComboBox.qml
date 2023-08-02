import QtQuick
import QtQuick.Controls

// needed for dark mode on Android
ComboBox {
    id: control

    delegate: ItemDelegate {

        width: control.width

        highlighted: control.highlightedIndex === index

        contentItem: Text {
            text: modelData
                  ? modelData[control.textRole]
                    ? modelData[control.textRole]
                    : modelData
                  : model[control.textRole]
            color: isDark(bg.color) ? "white" : "black"
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            id: bg
            width: control.width
            implicitHeight: contentItem.implicitHeight
            color: highlighted ? palette.highlight : palette.window
        }
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: control.height / 3
        height: width * 0.75
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = text.color;
            context.fill();
        }
    }

    contentItem: Text {
        id: text
        leftPadding: 6
        rightPadding: control.indicator.width + control.spacing

        text: control.displayText
        font: control.font
        color: isDark(palette.button) ? "white" : "black"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 28
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: !down ? Qt.lighter(palette.button, 1.2) : palette.button
            }
            GradientStop {
                position: 0.2
                color: palette.button
            }
            GradientStop {
                position: 0.8
                color: !down ? Qt.darker(palette.button, 1.2) : palette.button
            }
        }
        border.width: control.hovered ? 1 : 0
        border.color: palette.button
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            readonly property int maxHeight: 300
            clip: true
            implicitHeight: contentHeight > maxHeight ? maxHeight : contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollBar.vertical: ScrollBar {
                policy: control.count > 8 ? Qt.ScrollBarAlwaysOn : Qt.ScrollBarAsNeeded // do not hide SB for logn lists
            }
         }
    }

}
