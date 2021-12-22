import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

//import Qt5Compat.GraphicalEffects

ApplicationWindow {
    width: 1024 //TODO: system default
    height: 800
    visible: true
    title: qsTr("Scrabble3D")

    function updateFieldSize() {
        fs = view2D.height < view2D.width
                ? view2D.height / (GamePlay.boardSize+2.5)
                : view2D.width / (GamePlay.boardSize+2.5)
    }

    property int fs: updateFieldSize();

    Action {
        id: acNewGame
        text: qsTr("New Game")
        property string tip: text
        shortcut: qsTr("Ctrl+G")
        icon.source: "qrc:///resources/newgame.png"
        onTriggered: {
            var letterlist = [];
            for (var i=0; i<config.letterSet.rowCount; i++) {
                letterlist.push(config.letterSet.getRow(i).letter);
                letterlist.push(config.letterSet.getRow(i).value);
                letterlist.push(config.letterSet.getRow(i).count);
            }
            GamePlay.startNewGame(["Heiko","Claudia","Sara","Ignacio"],
                                  config.numberOfLettersOnRack,
                                  config.is3D,
                                  config.fields,
                                  letterlist, //config.letterSet,
                                  true,
                                  50,
                                  7,
                                  10,
                                  true,
                                  0,
                                  10,
                                  3,
                                  false,
                                  false,
                                  true,
                                  true,
                                  0,
                                  10,
                                  true,
                                  0,
                                  30,
                                  10,
                                  10,
                                  50,
                                  false,
                                  false);
            board.updateLabelsModel();
            updateFieldSize();
        } //onTriggered
    }
    Action {
        id: acNextPlayer
        text: qsTr("Next Player")
        enabled: GamePlay.isAccepted
        property string tip: enabled ? text : GamePlay.getLastError()
        icon.source: "qrc:///resources/nextplayer.png"
        onTriggered: GamePlay.execNextPlayer()
    }
    Action {
        id: acExitApp
        text: qsTr("E&xit")
        onTriggered: Qt.quit();
    }
    Action {
        id: acConfiguration
        text: qsTr("Configuration")
        onTriggered: configDialog.visible = true
    }
    Action {
        id: acSaveGame
        text: qsTr("Save As...")
        onTriggered: {
            fileDialog.acceptLabel = qsTr("Save")
            fileDialog.fileMode = FileDialog.SaveFile
            fileDialog.title = qsTr("Save game")
            fileDialog.currentFile = "test.ssg"
            fileDialog.open()
        }
    }
    Action {
        id: acLoadGame
        text: qsTr("Load From...")
        onTriggered: {
            fileDialog.acceptLabel = qsTr("Open")
            fileDialog.fileMode = FileDialog.OpenFile
            fileDialog.title = qsTr("Load game")
            fileDialog.open()
        }
    }
    Action {
        id: acComputeMove
        text: qsTr("Compute Move")
        onTriggered: GamePlay.computeMove()
    }

    function doLoad(aFileName)
    {
        GamePlay.loadGame(aFileName)
        board.updateLabelsModel()
        updateFieldSize()
    }

    FileDialog {
        id: fileDialog
        //TODO: debug
//        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: [qsTr("Scrabble3D savegame (*.ssg)"), qsTr("All files (*.*)")]
        onAccepted: fileMode == FileDialog.OpenFile ?
                      doLoad(currentFile)
                    : GamePlay.saveGame(currentFile)
        //TODO: https://bugreports.qt.io/browse/QTBUG-56663
    }

    ScrDefaults {
        id: defaults
    }
/*
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: false

        Page {
            ColumnLayout {
                id: view2D
                anchors.fill: parent
                onWidthChanged: updateFieldSize()
                onHeightChanged: updateFieldSize()

                ScrBoard {
                    id: board
                    fieldSize: fs
                    Layout.fillWidth: true
                }
                ScrRack {
                    id: rack
                    anchors.leftMargin: fs
                    anchors.topMargin: (GamePlay.boardSize + 1.2) * fs
                    fieldSize: fs
                    Layout.fillWidth: true
                }
            }
            Image {
                id: imHamburger
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
                        MenuItem { action: acNewGame }
                        MenuItem { action: acNextPlayer }
                        MenuSeparator { }
                        MenuItem { action: acConfiguration }
                        MenuSeparator { }
                        MenuItem { action: acExitApp }
                    }
                }
            }
            RoundButton {
                id: rbActionButton
                width: 50
                height: 50
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: 25
                anchors.bottomMargin: 50
                action: GamePlay.isRunning ? acNextPlayer : acNewGame
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
                    text: actionButton.action.tip
                    visible: text.length === 0 ? false : actionButton.hovered
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
        ScrCube {
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Board")
        }
        TabButton {
            text: qsTr("Cube")
        }
//        TabButton {}
    }
*/

    menuBar: MenuBar {
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
        }

        Menu {
            id: helpMenu
            title: qsTr("&Help")
        }
    }
    header: ToolBar {
        id: toolBar
        background: Rectangle { color: "transparent" }
        RowLayout {
            spacing: 3
            ToolButton {
                id: btnNewGame
                action: acNewGame;
                text: "";
                ToolTip {
                    text: acNewGame.tip
                    visible: text.length === 0 ? false : btnNewGame.hovered
                    delay: 1000
                    timeout: 5000
                }
            }
            Button { //Toolbutton shows no proper feedback on disabled state
                id: btnNextPlayer
                action: acNextPlayer
                text: "";
                ToolTip {
                    text: acNextPlayer.tip
                    visible: text.length === 0 ? false : btnNextPlayer.hovered
                    delay: 1000
                    timeout: 5000
                }
            }
            ToolButton {
                id: btnComputeMove
                action: acComputeMove
            }
        }
    }
    ColumnLayout {
        id: contentArea
        anchors.fill: parent
        SplitView {
            id: leftPane
            Layout.fillWidth: true
            Layout.fillHeight: true
            onResizingChanged: updateFieldSize()

            Item {
                id: view2D
                Layout.fillWidth: true
                Layout.fillHeight: true
                SplitView.preferredWidth: parent.width * 5/8
                SplitView.minimumWidth: 100
                ScrBoard {
                    id: board
                    anchors.fill: parent
                    fieldSize: fs
                }
                ScrRack {
                    id: rack
                    anchors.fill: parent
                    anchors.leftMargin: fs
                    anchors.topMargin: (GamePlay.boardSize + 1.2) * fs
                    fieldSize: fs
                }
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
                        ScrMessages {}
                        ScrStatistics {}
                    }
                    TabBar {
                        id: tabs
                        TabButton { text: qsTr("Messages") }
                        TabButton { text: qsTr("Statistics") }
                    }
                }
            }
        }
    }
    footer: ScrStatusbar {}

    Window {
        id: configDialog
        visible: false //show per action acConfiguration
        modality: Qt.ApplicationModal
        flags: Qt.Dialog
        title: qsTr("Configuration")
        width: 800
        height: 640

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            ScrConfig {
                id: config
                focus: true
                Keys.onEscapePressed: configDialog.close()
            }
            Rectangle {
                height: 36
                Layout.fillWidth: true
                color: "transparent"
                Button {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 10
                    text: qsTr("Close")
                    onClicked: configDialog.close()
                }
            }
        }
    }
}
