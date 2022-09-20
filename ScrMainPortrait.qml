import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout {
    property alias board: board
    property alias messages: messages
    property alias cube: cube

    anchors.fill: parent

    ToolBar {
        id: toolBar
        Layout.margins: 6
        Layout.fillWidth: true
        background: Rectangle { color: "transparent" }
        RowLayout {
            spacing: 12
            anchors.fill: parent
            ToolbarButton {
                id: btnNewGame
                action: acNewGame
                size: 38
            }
            ToolbarButton {
                id: btnNextPlayer
                action: acNextPlayer
                size: 38
            }
            ToolbarButton {
                id: btnChallenge
                action: acChallenge
                size: 38
            }

            ToolbarButton {
                id: btnComputeMove
                action: acComputeMove
                size: 38
            }
            Row {
                id: spinner // Spinbox font color is invisible on Android
                property int max: GamePlay.bestMoveCount
                onMaxChanged: value = 0
                property int value: 0
                onValueChanged: GamePlay.placeBestMove(spinner.value)

                ToolbarButton {
                    id: tbSpinBoxDown
                    size: 38
                    enabled: lbSpinBox.enabled && spinner.value > 0
                    onClicked: spinner.value--
                    icon.source: "qrc:///resources/down.png"
                }
                Label {
                    id: lbSpinBox
                    enabled: spinner.max > 0
                    width: 50
                    height: 38
                    font.pixelSize: 20
                    font.weight: Font.Bold
                    color: tbSpinBoxDown.icon.color
                    text: spinner.value
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                }
                ToolbarButton {
                    id: tbSpinBoxUp
                    size: 38
                    enabled: lbSpinBox.enabled && spinner.value < spinner.max
                    onClicked: spinner.value++
                    icon.source: "qrc:///resources/up.png"
                }
            }

            Rectangle { //dummy spacer
                Layout.fillWidth: true
                color: "transparent"
            }
            ToolbarButton {
                size: 38
                icon.source: "qrc:///resources/hamburger.png"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        contextMenu.popup()
                    }
                    Menu {
                        id: contextMenu
                        MenuItem { action: acNewGame }
                        MenuItem { action: acNextPlayer }
                        MenuItem { action: acChallenge }
                        MenuItem { action: acComputeMove }
                        MenuItem { action: acDictionary }
                        MenuSeparator { }
                        MenuItem { action: acLoadGame }
                        MenuItem { action: acSaveGame }
                        MenuSeparator { }
                        MenuItem { action: acNetwork; icon.source: "" }
                        MenuSeparator { }
                        MenuItem { action: acConfiguration }
                        Menu {
                            title: qsTr("View Mode")
                            MenuItem { action: acAutomaticView }
                            MenuItem { action: acLandscapeView }
                            MenuItem { action: acPortraitView }
                        }
                        MenuItem { action: acAbout }
                        MenuSeparator { }
                        MenuItem { action: acExit }
                    }
                }
            }
        }
    }

    SwipeView {
        id: swipeView

        Layout.fillHeight: true
        Layout.fillWidth: true

        currentIndex: footerBar.currentIndex
        interactive: false //touch-move should rotate the cube
        Item { ScrBoard { id: board } }
        ScrCube { id: cube; visible: GamePlay.is3D }
        ScrMessages { id: messages }
        ScrGameCourse { id: statistics }
        ScrPlayers { id: players; visible: GamePlay.isConnected }
    }
    TabBar {
        id: footerBar
        Layout.fillWidth: true
        currentIndex: swipeView.currentIndex
        onCurrentIndexChanged: if (currentIndex < 3) // Board=0/Cube=1/Messages=2
                                   messages.newMessage = false
        TabButton {
            text: qsTr("Board")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
        TabButton {
            text: qsTr("Cube")
            visible: GamePlay.is3D
            width: visible ? contentItem.implicitWidth + leftPadding + rightPadding
                           : 0 //non-visible keeps empty space
        }
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
        //TODO: main views: add statistics
    }

    ScrStatusbar { id: footerBarPt }
}
