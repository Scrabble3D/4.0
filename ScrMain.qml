import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Page {
    property int fs: parent.height < parent.width ? parent.height / (GamePlay.boardSize+2.5)
                                                  : parent.width / (GamePlay.boardSize+2.5)

    ScrBoard {
        id: board
        fieldSize: fs
    }
    ScrRack {
        fieldSize: fs
    }
    //todo v
    onWidthChanged: updateFieldSize();
    onHeightChanged: updateFieldSize();
    function updateFieldSize() {
        fs = parent.height < parent.width
                ? parent.height / (GamePlay.boardSize+2.5)
                : parent.width / (GamePlay.boardSize+2.5)
    }

    Action {
        id: acNewGame
        text: qsTr("New Game")
        property string tip: text
        shortcut: qsTr("Ctrl+G")
        icon.source: "qrc:///resources/newgame.png"
        onTriggered: {
            var letterlist = [];
            for (var i=0; i<config.letterSet.rowCount; i++) {
                letterlist.push(config.letterSet.getRow(i).letter);
                letterlist.push(config.letterSet.getRow(i).value);
                letterlist.push(config.letterSet.getRow(i).count);
            }
            GamePlay.startNewGame(["Heiko"],
                                  config.numberOfLettersOnRack,
                                  config.is3D,
                                  config.fields,
                                  letterlist, //config.letterSet,
                                  true,
                                  50,
                                  7,
                                  10,
                                  true,
                                  0,
                                  10,
                                  3,
                                  false,
                                  false,
                                  true,
                                  true,
                                  0,
                                  10,
                                  true,
                                  0,
                                  30,
                                  10,
                                  10,
                                  50,
                                  false,
                                  false);
            fs = parent.height < parent.width ? parent.height / (GamePlay.boardSize+2.5)
                                              : parent.width / (GamePlay.boardSize+2.5)
            board.updateLabelsModel();
        } //onTriggered
    }
    Action {
        id: acNextPlayer
        text: qsTr("Next Player")
        property string tip: enabled ? text : GamePlay.getLastError()
        enabled: GamePlay.isAccepted
        icon.source: "qrc:///resources/nextplayer.png"
        onTriggered: GamePlay.execNextPlayer()
    }
    Action {
        id: acExitApp
        text: qsTr("E&xit")
        onTriggered: Qt.quit();
    }

    Image {
        width: 24
        height: 24
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 16
        anchors.topMargin: 16
        source: "qrc:///resources/hamburger.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                contextMenu.popup()
            }
            Menu {
                id: contextMenu
                MenuItem { action: acNewGame }
                MenuItem { action: acNextPlayer }
                MenuSeparator { }
                MenuItem { action: acExitApp }
            }
        }
    }

    RoundButton {
        id: actionButton
        width: 50
        height: 50
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 25
        anchors.bottomMargin: 50
        action: GamePlay.isRunning ? acNextPlayer : acNewGame
        icon.color: "white"
        display: AbstractButton.IconOnly
        background: Rectangle {
            anchors.fill: parent
            radius: width / 2
            gradient: Gradient {
                GradientStop { position: 0.0; color: actionButton.action.enabled ? "darkgreen" : "darkgrey"}
                GradientStop { position: 1.0; color: actionButton.action.enabled ? "limegreen" : "lightgrey"}
            }
        }
        ToolTip {
            text: actionButton.action.tip
            visible: text.length === 0 ? false : actionButton.hovered
            delay: 1000
            timeout: 5000
        }
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: actionButton.down ? "transparent" : "darkgrey"
            horizontalOffset: 3
            verticalOffset: 3
            radius: 10
        }
    }
}
