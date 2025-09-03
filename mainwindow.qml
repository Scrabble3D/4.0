import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform // native dialogs, instead of import QtQuick.Dialogs
import QtCore

ApplicationWindow {
    id: scrabble3D
    width: 1024
    height: 800
    visible: true
    title: "Scrabble3D"

    background: Rectangle {
        anchors.fill: parent
        color: config.myPalette.window
    }
    property int gcSplitter: -1 //preferredHeight of gamecourse:totalStat
    //FIXME: main: Settings: Failed to initialize QSettings set: QList("organizationName", "organizationDomain")
/*
    Settings {
        id: settings
        location: GamePlay.config()
        property alias x: scrabble3D.x
        property alias y: scrabble3D.y
        property alias width: scrabble3D.width
        property alias height: scrabble3D.height
        property alias showTips: config.showTips
        property alias gcSplitter: scrabble3D.gcSplitter
    }
*/
    property alias main: mainLoader.item

    property color iconColor: isDark(config.myPalette.window) ? "white" : "black"

    //helper function used in ScrMessages, ScrConfigDictionary, ScrWordSearch...
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
            if (isLoading) newgame.done(Dialog.Accepted)
        }
        function onLoadGame(fileName) {
            config.loadConfig("")
            doLoad(fileName)
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
        enabled: !GamePlay.isComputing
        onTriggered: GamePlay.isConnected
                     ? GamePlay.syncNewGame(config.getConfigData(false))
                     : newgame.open()
    }
    Action {
        id: acNextPlayer
        text: qsTr("Next Player")
        shortcut: StandardKey.NextChild
        enabled: GamePlay.isRunning && GamePlay.isAccepted && !GamePlay.isHistory && !GamePlay.isComputing
        property string tip: enabled ? text : GamePlay.getLastError()
        icon.source: "qrc:///resources/nextplayer.png"
        icon.color: iconColor
        onTriggered: GamePlay.isConnected
                     ? GamePlay.syncNextPlayer()
                     : GamePlay.nextPlayer()
    }
    Action {
        id: acChallenge
        text: qsTr("Object Placed Word")
        icon.source: "qrc:///resources/challenge.png"
        icon.color: iconColor
        enabled: GamePlay.isRunning && GamePlay.isChallenge
        onTriggered: GamePlay.syncChallengeWord()
    }
    Action {
        id: acExit
        text: qsTr("Exit")
        shortcut: StandardKey.Quit
        onTriggered: Qt.quit();
    }
    Action {
        id: acConfiguration
        text: qsTr("Configuration...")
        shortcut: StandardKey.Preferences
        onTriggered: config.show()
    }
    Action {
        id: acSaveGame
        text: qsTr("Save As...")
        shortcut: StandardKey.SaveAs
        enabled: GamePlay.isInitialized && !GamePlay.isConnected
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
        onTriggered:
            if (!GamePlay.isConnected) {
                fileDialog.acceptLabel = qsTr("Open")
                fileDialog.fileMode = FileDialog.OpenFile
                fileDialog.title = qsTr("Load game")
                fileDialog.open()
            } else
                GamePlay.getRemoteGames()
    }
    Action {
        id: acComputeMove
        text: qsTr("Compute Move")
        shortcut: StandardKey.Find
        property string tip: text
        enabled: !GamePlay.isConnected && !GamePlay.isComputing &&
                 ((GamePlay.isRunning && GamePlay.computeResults === 0) ||
                  (GamePlay.isHistory))

        icon.source: "qrc:///resources/wheelchair.png"
        icon.color: iconColor
        onTriggered: GamePlay.computeMove()
    }
    Action {
        id: acNetwork
        checked: GamePlay.isConnected
        text: GamePlay.isConnected
              ? qsTr("Disconnect from game server")
              : qsTr("Connect to game server")
        icon.source: GamePlay.isConnected
                     ? "qrc:///resources/netw_disco.png"
                     : "qrc:///resources/netw_connect.png"
        icon.color: iconColor
        onTriggered: GamePlay.isConnected
                     ? GamePlay.disconnect()
                     : network.open()
    }
    ActionGroup {
        id: acViewType
        exclusive: true
        Action {
            id: acAutomaticView
            checked: Qt.platform.os === "android"
            checkable: true
            text: qsTr("Automatic")
        }
        Action {
            id: acLandscapeView
            checked: Qt.platform.os !== "android"
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
        text: qsTr("Word Search")
        enabled: !GamePlay.isConnected || !GamePlay.isRunning
        onTriggered: dictionary.open()
    }
    Action {
        id: acUpdate
        text: qsTr("Check for updates")
        onTriggered: GamePlay.checkForUpdates()
    }
    Action {
        id: acAbout
        text: qsTr("About Scrabble3D")
        onTriggered: about.open()
    }
    Action {
        id: acCambioSecco
        text: qsTr("Cambio Secco")
        icon.source: "qrc:///resources/cambiosecco.png"
        enabled: GamePlay.cambioSecco
        onTriggered: GamePlay.isConnected
                     ? GamePlay.syncCambioSecco()
                     : GamePlay.doCambioSecco()
    }

    function doLoad(aFileName)
    {
        GamePlay.loadGame(aFileName)
        main.board.updateLabelsModel()
        main.board.updateFieldSize() //changing the number of fields should result in resizing
        main.board.jokerPicker.updatePickerModel(); //setup letterlist
    }

    FileDialog {
        id: fileDialog
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: [qsTr("Scrabble3D savegame (*.ssg)"), qsTr("All files (*)")]
        defaultSuffix: "ssg"
        onAccepted: fileMode === FileDialog.OpenFile
                        ? doLoad(file)
                        : GamePlay.saveGame(file)}

    ScrDefaults    { id: defaults }
    ScrNewGame     { id: newgame }
    ScrWordSearch  { id: dictionary }
    ScrNetwork     { id: network }
    ScrAbout       { id: about }
    ScrRemoteGames { id: remotegames}
    ScrConfig      { id: config } //needs to come last trigger system palette change

    onWidthChanged: if (acAutomaticView.checked) {
        height > width
            ? mainLoader.state = "portrait"
            : mainLoader.state = "landscape"
    }

    Loader {
        id: mainLoader
        anchors.fill: parent
        asynchronous: true
        state: height > width ? "portrait" : "landscape"
        onLoaded: main.board.updateFieldSize() //changing the number of fields should result in resizing
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
