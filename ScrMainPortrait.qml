import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
//TODO: mobile: shadow on roundbutton
import Qt5Compat.GraphicalEffects

ColumnLayout {
    property alias board: board
    property alias messages: messages

    anchors.fill: parent
    spacing: 0

//    MenuBar { id: menuBar; visible: false }
//    ToolBar { id: toolBar; visible: false }

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

    Image {
        id: imHamburger
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        Layout.topMargin: -swipeView.height + 16
        Layout.rightMargin: 16

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
                MenuItem { action: acComputeMove }
                MenuSeparator { }
                MenuItem { action: acConfiguration }
                MenuSeparator { }
                MenuItem { action: acLandscapeView }
                MenuItem { action: acPortraitView }
                MenuSeparator { }
                MenuItem { action: acAbout }
                MenuSeparator { }
                MenuItem { action: acExitApp }
            }
        }
    }
    RoundButton {
        id: rbActionButton
        implicitWidth: 50
        implicitHeight: 50

        Layout.alignment: Qt.AlignRight | Qt.AlignBottom
        Layout.bottomMargin: 24
        Layout.rightMargin: 16

        action: GamePlay.isRunning ? acNextPlayer : acNewGame
        icon.color: "white"

        display: AbstractButton.IconOnly
        background: Rectangle {
            anchors.fill: parent
            radius: width / 2
            gradient: Gradient {
                GradientStop { position: 0.0; color: rbActionButton.action.enabled ? "darkgreen" : "darkgrey"}
                GradientStop { position: 1.0; color: rbActionButton.action.enabled ? "limegreen" : "lightgrey"}
            }
        }
        ToolTip {
            text: rbActionButton.action.tip
            visible: text.length === 0 ? false : rbActionButton.hovered
            delay: 1000
            timeout: 5000
        }
/*        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: rbActionButton.down ? "transparent" : "darkgrey"
            horizontalOffset: 3
            verticalOffset: 3
            radius: 10
        } */
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
        TabButton { text: qsTr("Statistics") }
    }

}
