import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

RowLayout {
    ProgressBar {
        id: statProgress
        Layout.preferredHeight: 25
        Layout.preferredWidth: 100
        Layout.margins: 2
        value: 0.5
//            indeterminate: true
    }
    Text {
        id: statMessages
        Layout.preferredHeight: 25
        Layout.fillWidth: true

        width: parent.width
        height: parent.height
        leftPadding: 4
        verticalAlignment: Text.AlignVCenter
        clip: true
        elide: Text.ElideRight
        text: GamePlay.lastError
    }
    Repeater {
        id: statPlayers
        model: GamePlay.numberOfPlayers
        ShadowText {
            Layout.preferredHeight: 25
            Layout.preferredWidth: shadowWidth + shadowPadding

            shadowColor: config.colorplayers
                         ? Qt.lighter(config.playercolors.get(index).itemColor)
                         : "transparent"
            shadowText: GamePlay.getPlayerName(index);
            active: index == GamePlay.currentPlayer;
        }

    }
}
