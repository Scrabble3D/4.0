import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {

    property alias board: board
    property alias cube: cube
    property alias messages: messages

    anchors.fill: parent
    spacing: 0

    MenuBar {
        id: menuBar
        Menu {
            id: fileMenu
            title: qsTr("File")
            MenuItem { action: acLoadGame }
            MenuItem { action: acSaveGame }
            MenuSeparator { }
            MenuItem { action: acExitApp }
        }
        Menu {
            id: editMenu
            title: qsTr("&Game")
            MenuItem { action: acNewGame }
            MenuItem { action: acNextPlayer }
        }
        Menu {
            id: viewMenu
            title: qsTr("&Configuration")
            MenuItem { action: acConfiguration }
            MenuSeparator { }
            MenuItem { action: acDictionary }
        }
        Menu {
            id: helpMenu
            title: qsTr("&Help")
            MenuItem { action: acLandscapeView }
            MenuItem { action: acPortraitView }
            MenuSeparator { }
            MenuItem { action: acAbout }
        }
    }
    ToolBar {
        id: toolBar
        background: Rectangle { color: "transparent" }
        RowLayout {
            spacing: 3
            ToolbarButton {
                id: btnNewGame
                action: acNewGame
                text: ""
                ToolTip {
                    text: acNewGame.tip
                    visible: text.length === 0 ? false : btnNewGame.hovered
                    delay: 1000
                    timeout: 5000
                }
            }
            ToolbarButton {
                id: btnNextPlayer
                action: acNextPlayer
                text: ""
                ToolTip {
                    text: acNextPlayer.tip
                    visible: text.length === 0 ? false : btnNextPlayer.hovered
                    delay: 1000
                    timeout: 5000
                }
            }
            ToolbarButton {
                id: btnComputeMove
                text: ""
                action: acComputeMove
            }
            SpinBox {
                id: sbBestMoveSpinner
                to: GamePlay.bestMoveCount
                enabled: to > 0
                editable: true
                onValueChanged: GamePlay.placeBestMove(value)
                onToChanged: value = 0
            }
        }
    }

    SplitView {
        id: mainView
        Layout.fillHeight: true
        Layout.fillWidth: true
        Item {
            id: leftPane
            SplitView.preferredWidth: parent.width * 5/8
            SplitView.minimumWidth: 100
            ScrBoard { id: board }
        }
        SplitView {
            id: rightPane
            orientation: Qt.Vertical
            ScrCube {
                id: cube
                visible: GamePlay.is3D
                SplitView.preferredHeight: parent.height * 1/3
            }
            ColumnLayout {
                StackLayout {
                    id: infostack
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    currentIndex: tabs.currentIndex
                    ScrMessages { id: messages }
                    ScrGameCourse { id: statistics }
                }
                TabBar {
                    id: tabs
                    TabButton { text: qsTr("Messages") }
                    TabButton { text: qsTr("Statistics") }
                }
            }
        }
    }

    ScrStatusbar { id: footerBar }
}


