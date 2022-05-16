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
                        MenuItem { action: acComputeMove }
                        MenuSeparator { }
                        MenuItem { action: acSaveGame }
                        MenuItem { action: acLoadGame }
                        MenuItem { action: acConfiguration }
                        MenuSeparator { }
                        MenuItem { action: acAutomaticView }
                        MenuItem { action: acLandscapeView }
                        MenuItem { action: acPortraitView }
                        MenuSeparator { }
                        MenuItem { action: acAbout }
                        MenuSeparator { }
                        MenuItem { action: acExitApp }
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
        ScrCube {
            id: cube
            visible: GamePlay.is3D
        }
        ScrMessages { id: messages }
        ScrGameCourse { id: statistics }
    }

    TabBar {
        id: footerBar
        Layout.fillWidth: true
        currentIndex: swipeView.currentIndex

        TabButton { text: qsTr("Board") }
        TabButton {
            text: qsTr("Cube")
            visible: GamePlay.is3D
            width: visible ? undefined : 0 //non-visible keeps empty space
        }
        TabButton { text: qsTr("Messages") }
        TabButton { text: qsTr("Game course") }
        //TODO: main views: add statistics
    }
    ScrStatusbar { id: footerBarPt }
}
