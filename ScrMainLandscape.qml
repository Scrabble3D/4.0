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
            title: qsTr("&File")
            MenuItem { action: acLoadGame }
            MenuItem { action: acSaveGame }
            MenuSeparator { }
            MenuItem { action: acExit }
        }
        Menu {
            id: editMenu
            title: qsTr("&Game")
            MenuItem { action: acNewGame }
            MenuItem { action: acNextPlayer }
            MenuItem { action: acChallenge }
            MenuItem { action: acComputeMove }
            MenuSeparator { }
            MenuItem { action: acNetwork; text: qsTr("Network") }
        }
        Menu {
            id: viewMenu
            title: qsTr("&Configuration")
            MenuItem { action: acConfiguration }
            MenuItem { action: acDictionary }
            MenuSeparator { }
            Menu {
                //TODO: mainform: view type actions as true radiobuttons; menuitemgroup doesnt work
                title: qsTr("&View Mode")
                MenuItem { action: acAutomaticView }
                MenuItem { action: acLandscapeView }
                MenuItem { action: acPortraitView }
            }
        }
        Menu {
            id: helpMenu
            title: qsTr("&Help")
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
                id: btnChallenge
                action: acChallenge
                text: ""
                ToolTip {
                    text: acChallenge.text
                    visible: text.length === 0 ? false : btnChallenge.hovered
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
            ToolbarButton {
                id: btnNetwork
                action: acNetwork
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
                    onWidthChanged:
                        if (players.visible) players.treeview.forceLayout() //update delegate width
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    currentIndex: tabs.currentIndex
                    ScrMessages { id: messages }
                    ScrGameCourse { id: statistics }
                    ScrPlayers { id: players; visible: GamePlay.isConnected }
                }
                TextField {
                    id: chatMsg
                    Layout.fillWidth: true
                    visible: GamePlay.isConnected
                    text: ""
                    placeholderText: qsTr("Enter a message to the group or use /help")
                    Keys.onReturnPressed: {
                        GamePlay.chat(text)
                        chatMsg.text = ""
                    }
                }
                TabBar {
                    id: tabs
                    onCurrentIndexChanged:
                        if (currentIndex === 0) messages.newMessage = false
                    TabButton {
                        text: qsTr("Messages")
                        width: contentItem.implicitWidth + leftPadding + 2*rightPadding
                        indicator: Rectangle {
                            property int size: 5
                            width: size; height: size; radius: size
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.rightMargin: 2
                            anchors.topMargin: 2
                            color: "#FF0000" //config.myPalette.highlight
                            visible: messages.newMessage
                        }
                    }
                    TabButton {
                        text: qsTr("Game course")
                        width: contentItem.implicitWidth + leftPadding + rightPadding
                    }
                    TabButton {
                        text: qsTr("Players")
                        width: contentItem.implicitWidth + leftPadding + rightPadding
                        visible: GamePlay.isConnected
                    }
                }
            }
        }
    }

    ScrStatusbar { id: footerBar }
}


