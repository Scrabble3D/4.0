import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Page {
    property int fs: swipeView.height < swipeView.width ? swipeView.height / (config.fieldCount+2.5)
                                                        : swipeView.width / (config.fieldCount+2.5)

    ScrBoard {
        id: board
        fieldSize: fs
    }
    ScrRack {
        fieldSize: fs
    }

    Action {
        id: newGame
        text: "New Game"
        shortcut: tr("Ctrl+G")
        icon.source: "qrc:///resources/newgame.png"
        onTriggered: game.startNewGame()
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
                MenuItem { text: "Next Player" }
                MenuItem { text: "Tip" }
            }
        }
    }

    Rectangle {
        color: "forestgreen"
        width: 50
        height: 50
        radius: width / 2
        anchors.left: board.right
        anchors.top: board.bottom
        Image {
            anchors.centerIn: parent
            source: "qrc:///resources/newgame.png"
        }
        MouseArea {
            id: mouseArea
            hoverEnabled: true
            anchors.fill: parent
            onClicked: game.startNewGame()
        }
        ToolTip {
            text: qsTr("Start a new game")
            visible: mouseArea.containsMouse
            delay: 1000
            timeout: 5000
        }
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: mouseArea.pressedButtons & Qt.LeftButton ? "transparent" : "darkgrey"
            horizontalOffset: 3
            verticalOffset: 3
            radius: 10
        }
    }
}
