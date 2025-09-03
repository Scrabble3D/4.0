import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {

    property alias board: board
    property alias cube: cube
    property alias messages: messages

    anchors.fill: parent
    spacing: 0

    component LongMenuItem: MenuItem {
        onImplicitWidthChanged: {
            if (menu.contentWidth < implicitWidth)
                menu.contentWidth = implicitWidth + 12
        }
    }

    MenuBar {
        id: menuBar
        Menu {
            id: fileMenu
            title: qsTr("&File")
            LongMenuItem { action: acLoadGame }
            LongMenuItem { action: acSaveGame }
            MenuSeparator { }
            LongMenuItem { action: acExit }
        }
        Menu {
            id: editMenu
            title: qsTr("&Game")
            LongMenuItem { action: acNewGame }
            LongMenuItem { action: acNextPlayer }
            LongMenuItem { action: acCambioSecco }
            LongMenuItem { action: acChallenge }
            LongMenuItem { action: acComputeMove }
            MenuSeparator { }
            LongMenuItem { action: acNetwork }
        }
        Menu {
            id: viewMenu
            title: qsTr("&Configuration")
            LongMenuItem { action: acConfiguration }
            LongMenuItem { action: acDictionary }
            MenuSeparator { }
            Menu {
                title: qsTr("&View Mode")
                RadioButton {
                    action: acAutomaticView
                    visible: Qt.platform.os === "android"
                    implicitHeight: Qt.platform.os === "android" ? implicitHeight : 0
                }
                RadioButton { action: acLandscapeView }
                RadioButton { action: acPortraitView }
            }
//            MenuItem { action: acUpdate } //testing purpose
        }
        Menu {
            id: helpMenu
            title: qsTr("&Help")
            LongMenuItem { action: acAbout }
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
                    text: acNextPlayer.text
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
                ToolTip {
                    text: acComputeMove.text
                    visible: text.length === 0 ? false : btnComputeMove.hovered
                    delay: 1000
                    timeout: 5000
                }
            }
            SpinBox {
                id: sbBestMoveSpinner
                to: GamePlay.computeResults
                enabled: to > 0
                editable: true
                onValueChanged: GamePlay.placeBestMove(value)
                onToChanged: value = 0
                ToolTip {
                    text: qsTr("Iterate through found words")
                    visible: text.length === 0 ? false : sbBestMoveSpinner.hovered
                    delay: 1000
                    timeout: 5000
                }
            }
            ToolbarButton {
                id: btnNetwork
                action: acNetwork
                text: ""
                ToolTip {
                    text: acNetwork.text
                    visible: text.length === 0 ? false : btnNetwork.hovered
                    delay: 1000
                    timeout: 5000
                }
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
                    background: Rectangle {
                        color: "transparent"
                    }
                    onCurrentIndexChanged:
                        if (currentIndex === 0) messages.newMessage = false
                    TabButton {
                        text: qsTr("Messages")
                        width: contentItem.implicitWidth + leftPadding + 2*rightPadding
                        indicator: Rectangle { //showing a red dot on new messages
                            property int size: 5
                            width: size; height: size; radius: size
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.rightMargin: 2
                            anchors.topMargin: 2
                            color: "#FF0000"
                            visible: messages.newMessage && tabs.currentIndex !== 0
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
                        onVisibleChanged: visible ? tabs.setCurrentIndex(2) : tabs.setCurrentIndex(0)
                    }
                }
            }
        }
    }

    ScrStatusbar { id: footerBar }
}


