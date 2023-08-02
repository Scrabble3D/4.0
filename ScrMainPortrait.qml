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
                text: ""
            }
            ToolbarButton {
                id: btnNextPlayer
                action: acNextPlayer
                size: 38
                text: ""
            }
            ToolbarButton {
                id: btnChallenge
                action: acChallenge
                size: 38
                text: ""
            }
            ToolbarButton {
                id: btnComputeMove
                action: acComputeMove
                size: 38
                text: ""
            }
            Row {
                id: spinner
                property int max: GamePlay.computeResults
                onMaxChanged: value = 0
                property int value: 0
                onValueChanged: GamePlay.placeBestMove(spinner.value)

                ToolbarButton {
                    id: tbSpinBoxDown
                    size: 38
                    text: ""
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
                    text: ""
                    enabled: lbSpinBox.enabled && spinner.value < spinner.max
                    onClicked: spinner.value++
                    icon.source: "qrc:///resources/up.png"
                }
            }
            Rectangle { //dummy spacer
                Layout.fillWidth: true
                color: "transparent"
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
    RoundButton {
        id: actionButton
        implicitWidth: 50
        implicitHeight: 50
        Layout.alignment: Qt.AlignRight
        Layout.topMargin: -50
        Layout.rightMargin: 10
        display: AbstractButton.IconOnly
        icon.width: 32
        icon.height: 32
        icon.source: "qrc:///resources/hamburger.png"
        onPressed: contextMenu.popup()
        background: Rectangle {
            anchors.fill: parent
            radius: width / 2
            gradient: Gradient {
                GradientStop {
                    position: 1.0
                    color: "darkgreen"
                }
                GradientStop {
                    position: 0.0
                    color: "limegreen"
                }
            }
        }
        Menu {
            id: contextMenu
            MenuItem { action: acNewGame }
            MenuItem { action: acNextPlayer }
            MenuItem { action: acCambioSecco }
            MenuItem { action: acChallenge }
            MenuItem { action: acComputeMove }
            MenuSeparator { }
            MenuItem { action: acLoadGame }
            MenuItem { action: acSaveGame }
            MenuSeparator { }
            MenuItem { action: acDictionary }
            MenuItem { action: acNetwork; icon.source: ""; text: qsTr("Network") }
            MenuSeparator { }
            MenuItem { action: acConfiguration }
            Menu {
                title: qsTr("View Mode")
//                RadioButton { action: acAutomaticView }
                RadioButton { action: acLandscapeView }
                RadioButton { action: acPortraitView }
            }
            MenuItem { action: acAbout }
            MenuSeparator { }
            MenuItem { action: acExit }
        }
    }
    TabBar {
        id: footerBar
        Layout.fillWidth: true
        currentIndex: swipeView.currentIndex
        onCurrentIndexChanged:
            if (currentIndex === 2) messages.newMessage = false
        TabButton {
            text: qsTr("Board")
            width: contentItem.implicitWidth + leftPadding + rightPadding
        }
        TabButton {
            text: qsTr("Cube")
            visible: GamePlay.is3D
            width: visible ? contentItem.implicitWidth + leftPadding + rightPadding
                           : 0 //non-visible keeps empty space
            onVisibleChanged: visible ? "" : footerBar.setCurrentIndex(0)
        }
        TabButton {
            text: qsTr("Messages")
            width: contentItem.implicitWidth + leftPadding + 2*rightPadding
            indicator: Rectangle { // red dot on new messages
                property int size: 5
                width: size; height: size; radius: size
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.rightMargin: 2
                anchors.topMargin: 2
                color: "#FF0000"
                visible: messages.newMessage && footerBar.currentIndex !== 2
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
            onVisibleChanged: visible ? footerBar.setCurrentIndex(4) : footerBar.setCurrentIndex(0)
        }
    }
    ScrStatusbar { id: footerBarPt }
}
