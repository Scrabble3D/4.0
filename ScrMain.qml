import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Page {
    property int fs: swipeView.height < swipeView.width ? swipeView.height / (game.boardSize+2.5)
                                                        : swipeView.width / (game.boardSize+2.5)

    ScrBoard {
        id: board
        fieldSize: fs
    }
    ScrRack {
        fieldSize: fs
    }

    Action {
        id: newGame
        text: qsTr("New Game")
        shortcut: qsTr("Ctrl+G")
        icon.source: "qrc:///resources/newgame.png"
        onTriggered: game.startNewGame()
    }
    Action {
        id: nextPlayer
        text: qsTr("Next Player")
        enabled: game.lastError < 2
        icon.source: "qrc:///resources/nextplayer.png"
        onTriggered: game.nextPlayer()
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
                MenuItem { action: newGame }
                MenuItem { action: nextPlayer }
                MenuItem { text: "Tip" }
            }
        }
    }

    RoundButton {
        id: actionButton
        width: 50
        height: 50
        anchors.left: board.right
        anchors.top: board.bottom
        action: game.isGameRunning ? nextPlayer :  newGame
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
            text: actionButton.action.text
            visible: actionButton.hovered
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
