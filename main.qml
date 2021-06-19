import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 650
    height: 800
    visible: true
    title: qsTr("Scrabble3D")

    ScrDefaults {
        id: defaults
    }
    ScrGame {
        id: game
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: false

        ScrMain {
        }
        ScrCube {
        }
        ScrConfig {
            id: config
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Board")
        }
        TabButton {
            text: "Cube"
        }
        TabButton {
            text: qsTr("Config")
        }
    }
}
