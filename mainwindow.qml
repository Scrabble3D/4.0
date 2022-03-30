import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.platform //Snative dialog
//import Qt5Compat.GraphicalEffects

ApplicationWindow {
    id: app
    width: 1024 //TODO: mainform: use system default for app size
    height: 800
    visible: true
    title: qsTr("Scrabble3D")

    property alias main: mainLoader.item

    property color iconColor: isDark(config.myPalette.window) ? "white" : "black"

    //helper function used in ScrMessages, ScrConfigDictionary, ScrWordSearch
    function isDark(aColor) {
        var temp = Qt.darker(aColor, 1) //Force conversion to color QML type object
        var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
        return temp.a > 0 && a >= 0.3
    }

    Action {
        id: acNewGame
        text: qsTr("New Game")
        property string tip: text
        shortcut: StandardKey.New
        icon.source: "qrc:///resources/newgame.png"
        icon.color: iconColor
        onTriggered: newgame.open()
    }
    Action {
        id: acNextPlayer
        text: qsTr("Next Player")
        shortcut: StandardKey.NextChild
        enabled: GamePlay.isRunning && GamePlay.isAccepted
        property string tip: enabled ? text : GamePlay.getLastError()
        icon.source: "qrc:///resources/nextplayer.png"
        icon.color: iconColor
        onTriggered: GamePlay.execNextPlayer()
    }
    Action {
        id: acExitApp
        text: qsTr("E&xit")
        shortcut: StandardKey.Quit
        onTriggered: Qt.quit();
    }
    Action {
        id: acConfiguration
        text: qsTr("Configuration")
        shortcut: StandardKey.Preferences
        onTriggered: (mainLoader.state === "portrait")
                     ? config.showFullScreen()
                     : config.show()
    }
    Action {
        id: acSaveGame
        text: qsTr("Save As...")
        shortcut: StandardKey.SaveAs
        onTriggered: {
            fileDialog.acceptLabel = qsTr("Save")
            fileDialog.fileMode = FileDialog.SaveFile
            fileDialog.title = qsTr("Save game")
            fileDialog.open()
        }
    }
    Action {
        id: acLoadGame
        text: qsTr("Load From...")
        shortcut: StandardKey.Open
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
        shortcut: StandardKey.Find
        property string tip: text
        enabled: GamePlay.isRunning
        icon.source: "qrc:///resources/wheelchair.png"
        icon.color: iconColor
        onTriggered: GamePlay.computeMove()
    }
    ActionGroup {
        id: acViewType
        //TODO: mainform: show mobile/desktop action as radiobuttons; ExclusiveGroup?
        exclusive: true
        Action {
            id: acLandscapeView
            checked: true
            checkable: true
            text: qsTr("Desktop")
            onCheckedChanged: if (acLandscapeView.checked) mainLoader.state = "landscape"
        }
        Action {
            id: acPortraitView
            checked: false
            checkable: true
            text: qsTr("Mobile")
            onCheckedChanged: if (acPortraitView.checked) mainLoader.state = "portrait"
        }
    }
    Action {
        id: acDictionary
        text: qsTr("Word search")
        onTriggered: dictionary.open()
    }
    Action {
        id: acAbout
        text: qsTr("About Scrabble3D")
        onTriggered: about.open()
    }

    function doLoad(aFileName)
    {
        GamePlay.loadGame(aFileName)
        main.board.updateLabelsModel()
        main.board.updateFieldSize() //changing the number of fields should result in resizing
        main.board.jokerPicker.updatePickerModel(); //setup letterlist
        if (config.bIs3D) main.cube.updateCubeModel()
    }

    FileDialog {
        id: fileDialog
        folder: GamePlay.documentPath() // StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: [qsTr("Scrabble3D savegame (*.ssg)"), qsTr("All files (*)")]
        defaultSuffix: "ssg"
        onAccepted: fileMode == FileDialog.OpenFile ?
                      doLoad(currentFile)
                    : GamePlay.saveGame(currentFile)
    }

    ScrDefaults { id: defaults }
    ScrNewGame  { id: newgame }
    ScrWordSearch {id: dictionary }
    ScrConfig   { id: config }
    ScrAbout    { id: about }

    Loader {
        id: mainLoader
        anchors.fill: parent
        asynchronous: true
        state: "landscape"
        onLoaded: {
            main.board.updateFieldSize() //changing the number of fields should result in resizing
        }
        states:
        [
            State
            {
                name: "landscape"
                PropertyChanges {
                    target: mainLoader
                    source: "ScrMainLandscape.qml"
                }
            },
            State
            {
                name: "portrait"
                PropertyChanges {
                    target: mainLoader
                    source: "ScrMainPortrait.qml"
                }
            }
        ]
    }

}
