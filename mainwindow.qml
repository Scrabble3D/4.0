import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//import QtQuick.Dialogs
import Qt.labs.platform //native dialog
//import Qt.labs.settings //TODO: main: save window position

ApplicationWindow {
    id: app //TODO: main: rename app
    width: 1024 //TODO: mainform: use system default for app size
    height: 800
    visible: true
    title: "Scrabble3D"
/*
    Settings {
        property alias x: app.x
        property alias y: app.y
        property alias width: app.width
        property alias height: app.height
    }
*/
    property alias main: mainLoader.item

    property color iconColor: isDark(config.myPalette.window) ? "white" : "black"

    //helper function used in ScrMessages, ScrConfigDictionary, ScrWordSearch
    function isDark(aColor) {
        var temp = Qt.darker(aColor, 1) //Force conversion to color QML type object
        var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
        return temp.a > 0 && a >= 0.3
    }

    Connections {
        target: GamePlay
        function onShowRemoteGames() {
            remotegames.open() }
        function onNewGame(isLoading) {
            newgame.open()
            if (isLoading) newgame.accept()
        }
        function onApplyConfig(configData) {
            config.applyConfig(configData)
            newgame.setPlayers(configData)
        }
    }

    Action {
        id: acNewGame
        text: qsTr("New Game")
        property string tip: text
        shortcut: StandardKey.New
        icon.source: "qrc:///resources/newgame.png"
        icon.color: iconColor
        onTriggered: GamePlay.isConnected
                     ? GamePlay.syncNewGame(config.getConfigData(false))
                     : newgame.open()
    }
    Action {
        id: acNextPlayer
        text: qsTr("Next Player")
        shortcut: StandardKey.NextChild
        enabled: GamePlay.isRunning && GamePlay.isAccepted
        property string tip: enabled ? text : GamePlay.getLastError()
        icon.source: "qrc:///resources/nextplayer.png"
        icon.color: iconColor
        onTriggered: GamePlay.isConnected
                     ? GamePlay.syncNextPlayer()
                     : GamePlay.nextPlayer()
    }
    Action {
        id: acExitApp
        text: qsTr("Exit")
        shortcut: StandardKey.Quit
        onTriggered: Qt.quit();
    }
    Action {
        id: acConfiguration
        text: qsTr("Configuration")
        shortcut: StandardKey.Preferences
        onTriggered: Qt.platform.os === "android"
                     ? config.showFullScreen()
                     : config.show()
    }
    Action {
        id: acSaveGame
        text: qsTr("Save As...")
        shortcut: StandardKey.SaveAs
        //TODO: main: filedialog on Android
        // https://www.volkerkrause.eu/2019/02/16/qt-open-files-on-android.html
        // https://forum.qt.io/topic/113328/how-to-convert-android-content-url-and-use-it-to-open-file/5
        // https://forum.qt.io/topic/113335/qfiledialog-getopenfilename-always-returns-empty-string-on-android/15
        enabled: Qt.platform.os !== "android" && GamePlay.isInitialized
        onTriggered: {
            fileDialog.acceptLabel = qsTr("Save")
            fileDialog.fileMode = FileDialog.SaveFile
            fileDialog.title = qsTr("Save game")
            fileDialog.open()
        }
    }
    function showLocalGames() {
        fileDialog.acceptLabel = qsTr("Open")
        fileDialog.fileMode = FileDialog.OpenFile
        fileDialog.title = qsTr("Load game")
        fileDialog.open()
    }
    Action {
        id: acLoadGame
        text: qsTr("Load From...")
        shortcut: StandardKey.Open
        enabled: Qt.platform.os !== "android" || GamePlay.isConnected
        onTriggered: GamePlay.isConnected
            ? GamePlay.getRemoteGames()
            : showLocalGames()
    }
    Action { //FIXME!: mainform disable compute action when not localplayer; check the various emits
        id: acComputeMove
        text: qsTr("Compute Move")
        shortcut: StandardKey.Find
        property string tip: text
        enabled: GamePlay.isRunning && GamePlay.bestMoveCount === 0
        icon.source: "qrc:///resources/wheelchair.png"
        icon.color: iconColor
        onTriggered: GamePlay.computeMove()
    }
    Action {
        id: acLocalize
        text: qsTr("Localize")
        onTriggered: GamePlay.localize("german.qm")
    }
    Action {
        id: acNetwork
        checked: GamePlay.isConnected
        checkable: true
        text: qsTr("Network")
        //TODO: main: update network icons
        icon.source: checked ? "qrc:///resources/netw_disco.png"
                             : "qrc:///resources/netw_connect.png"
        icon.color: iconColor
        onToggled: checked ? network.open() : GamePlay.disconnect()
    }

    ActionGroup {
        id: acViewType
        exclusive: true
        Action {
            id: acAutomaticView
            checked: true
            checkable: true
            text: qsTr("Automatic")
        }
        Action {
            id: acLandscapeView
            checked: false
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
        if (GamePlay.is3D) main.cube.updateCubeModel()
    }

    FileDialog {
        id: fileDialog
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        //TODO: main: clean-up
//            GamePlay.documentPath() // StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: [qsTr("Scrabble3D savegame (*.ssg)"), qsTr("All files (*)")]
        defaultSuffix: "ssg"
        onAccepted: fileMode === FileDialog.OpenFile
                        ? doLoad(file)
                        : GamePlay.saveGame(file)}

    ScrDefaults    { id: defaults }
    ScrNewGame     { id: newgame }
    ScrWordSearch  { id: dictionary }
    ScrConfig      { id: config }
    ScrAbout       { id: about }
    ScrNetwork     { id: network }
//    ScrPoll        { id: poll }
    ScrRemoteGames { id: remotegames}

    onWidthChanged: if (acAutomaticView.checked) {
        height > width
            ? mainLoader.state = "portrait"
            : mainLoader.state = "landscape"
    }

    Loader {
        id: mainLoader
        anchors.fill: parent
        asynchronous: true
        state: "landscape"//"portrait"//
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
