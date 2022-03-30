import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

RowLayout {
    property int tbHeight: 22
    property string tip: "";
    Rectangle {
        id: statProgress
        Layout.preferredHeight: tbHeight
        Layout.preferredWidth: 100
        Layout.margins: 1
        border.color: config.myPalette.mid
        color: config.myPalette.window
        Column {
            anchors.fill: parent
            spacing: 1
            Rectangle {
                property int computeProgress: GamePlay.computeProgress
                color: "skyblue"
                height: parent.height
                onComputeProgressChanged: {
                    width = Math.round(parent.width * computeProgress/100)
                    tip = qsTr("Progress: %1 %").arg(computeProgress)
                }
            }
            Rectangle {
                property int placedValue: GamePlay.placedValue
                height: parent.height
                color: "lightgreen"
                onPlacedValueChanged: {
                    width = Math.round(parent.width * placedValue/50)
                    tip = qsTr("Score: %1 points").arg(placedValue)
                }
            }
        }
        MouseArea {
            id: maProgress
            anchors.fill: parent
            hoverEnabled: true
        }
        ToolTip {
            id: tipProgress
            text: tip
            visible: (tip.length>0) && maProgress.containsMouse
            delay: 10
            timeout: 5000
        }
    }
    Rectangle {
        id: statMessages
        Layout.preferredHeight: tbHeight
        Layout.fillWidth: true
        Layout.margins: 1
        border.color: config.myPalette.mid
        color: config.myPalette.window
        Text {
            leftPadding: 4
            width: parent.width
            height: parent.height
            text: GamePlay.lastError
            verticalAlignment: Text.AlignVCenter
            clip: true
            elide: Text.ElideRight
        }
    }
    Repeater {
        id: statPlayers
        model: GamePlay.numberOfPlayers
        Layout.margins: 1
        ShadowText {
            Layout.preferredHeight: tbHeight
            Layout.preferredWidth: shadowWidth + shadowPadding

            color: config.myPalette.window
            shadowColor: config.bColoredPlayers
                         ? Qt.lighter(config.playercolors.get(index).itemColor)
                         : "transparent"
            shadowText: GamePlay.getPlayerName(index);
            active: index === GamePlay.currentPlayer;
        }

    }
}
