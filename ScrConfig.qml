import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.qmlmodels //TableModel
import Qt.labs.platform //Standardpath, native dialog

Window {
    visible: false //show per action acConfiguration
    modality: Qt.ApplicationModal
    flags: Qt.Dialog
    title: qsTr("Configuration")
    width: 680
    height: 540

    property bool bMarkers: true //ScrConfigBoard::cbMarkers.checked
    property bool bColoredPlayers: true //ScrConfigBoard::cbPlayerColors.checked
    property bool bIs3D: false //ScrConfigBoard::rb3D.checked

    property int numberOfLettersOnRack: 7 //ScrConfigLetter::sbPieces.value
    property int numberOfJokers: 2 // set in onValueChange of ScrConfigLetter::sbJoker.value
    property int numberOfPasses: 3 // set in onValueChange of ScrConfigRules::sbPasses
    property int timeControl: 0 // set in onToggled of ScrConfigRules::rbNoLimit/rbPerMove/rbPerGame
    property int timeControlValue: 10 // set in onToggled of ScrConfigRules::rbNoLimit/rbPerMove/rbPerGame
    property int bingoBonus: 50 // set in onToggled of ScrConfigRules::sbBingo

    property TableModel letterSet // ScrConfigLetter::tvLetterSet.model
    property alias colors: scrColors
    property alias playercolors: scrPlayerColors
    property var board: []

    property alias myPalette: myPalette

    property string boardName: configBoard.boardName
    property string lettersetName: configLetter.lettersetName
    property string dictionaryName: configDictionary.dictionaryName
    Component.onCompleted: {
        //fill color names with random color values and reset later to the default
        var cNames = [qsTr("Start"), qsTr("Default"),qsTr("2x letter"),qsTr("3x letter"),
                      qsTr("4x letter"),qsTr("2x word"),qsTr("3x word"),qsTr("4x word"),
                      qsTr("Pieces")]
        for (var i=0; i<cNames.length; i++)
            scrColors.append({"itemColor":"#000000","itemName":cNames[i]})
        cNames = [qsTr("First player"), qsTr("Second player"),
                  qsTr("Third player"), qsTr("Fourth player")]
        for (i=0; i<cNames.length; i++)
            scrPlayerColors.append({"itemColor":"#000000","itemName":cNames[i]})

        //load last session properties
        loadConfig("")
    }

    function saveConfig(fileName)
    {
        var configData = {}
        //board
        configData["cbMarkers"] = configBoard.cbMarkers.checked
        configData["rb3D"] = configBoard.rb3D.checked
        configData["board"] = board.toString()
        var aColors=[]
        for (var i = 0; i < scrColors.count; ++i)
            aColors.push(scrColors.get(i).itemColor.toString())
        configData["colors"] = aColors.toString()
        aColors=[]
        for (i = 0; i < scrPlayerColors.count; ++i)
            aColors.push(scrPlayerColors.get(i).itemColor.toString())
        configData["playercolors"] = aColors.toString()
        configData["cbPlayerColors"] = configBoard.cbPlayerColors.checked
        //letter
        configData["cbLetterSet"] = configLetter.cbLetterSet.currentIndex
        configData["sbJokers"] = configLetter.sbJoker.value
        configData["sbPieces"] = configLetter.sbPieces.value
        configData["sbRandoms"] = configLetter.sbRandoms.value
        configData["rbReadingDirectionLTR"] = configLetter.rbReadingDirectionLTR.checked
        //configRules
        configData["rbNoLimit"] = configRules.rbNoLimit.checked
        configData["rbPerMove"] = configRules.rbPerMove.checked
        configData["tiPerMove"] = configRules.tiPerMove.text
        configData["rbPerGame"] = configRules.rbPerGame.checked
        configData["tiPerGame"] = configRules.tiPerGame.text
        configData["rbGameEnd"] = configRules.rbGameEnd.checked
        configData["rbPenalty"] = configRules.rbPenalty.checked
        configData["sbPenaltyPoints"] = configRules.sbPenaltyPoints.value
        configData["sbPenaltyCount"] = configRules.sbPenaltyCount.value
        configData["cbGameEnd"] = configRules.cbGameEnd.checked
        configData["sbPasses"] = configRules.sbPasses.value
        //dictionary
        configData["dictionary"] = GamePlay.currentDicName()
        var aCat = []
        for (i = 1; i < configDictionary.categoriesRepeater.count; ++i)
            aCat.push(configDictionary.categoriesRepeater.itemAt(i).checked)
        configData["categories"] = aCat.toString()

        GamePlay.saveConfig(fileName, configData)
    }
    function loadConfig(fileName)
    {
        //reset calls applyConfig({})
        applyConfig( GamePlay.loadConfig(fileName) )
    }
    function applyConfig(configData)
    {
        function getConfigValue(key, def) {
            return configData[key] || def;
        }

        //board
        configBoard.cbMarkers.checked = getConfigValue("cbMarkers", "true") === "true"
        if (getConfigValue("rb3D", "false") === "true")
            configBoard.rb3D.checked = true
        else
            configBoard.rb2D.checked = true
        board = []
        board = getConfigValue("board", "6,1,1,2,1,1,1,6,1,1,1,2,1,1,6,1,5,1,1,1,3,1,1,1,3,1,1,1,5,1,1,1,5,1,1,1,2,1,2,1,1,1,5,1,1,2,1,1,5,1,1,1,2,1,1,1,5,1,1,2,1,1,1,1,5,1,1,1,1,1,5,1,1,1,1,1,3,1,1,1,3,1,1,1,3,1,1,1,3,1,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,6,1,1,2,1,1,1,0,1,1,1,2,1,1,6,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,3,1,1,1,3,1,1,1,3,1,1,1,3,1,1,1,1,1,5,1,1,1,1,1,5,1,1,1,1,2,1,1,5,1,1,1,2,1,1,1,5,1,1,2,1,1,5,1,1,1,2,1,2,1,1,1,5,1,1,1,5,1,1,1,3,1,1,1,3,1,1,1,5,1,6,1,1,2,1,1,1,6,1,1,1,2,1,1,6")
                   .split(",").map(function(t){return parseInt(t)})
        configBoard.checkDefault()
        if (configBoard.rb2D.checked)
            configBoard.sbSize.value = Math.sqrt(config.board.length)
        else
            configBoard.sbSize.value = Math.round(Math.cbrt(config.board.length))
        var aColors = getConfigValue("colors", "#c0c0c0,#008000,#00ffff,
                                                #0000ff,#000060,#ff6060,
                                                #ff0000,#600000,#ffff00").split(",")
        if (scrColors.count === aColors.length) {
            for (var i = 0; i < scrColors.count; ++i)
                scrColors.setProperty(i,"itemColor",Qt.color(aColors[i].trim()));
        } else
            console.warn("Unexpected number of colors in configuration")

        aColors = getConfigValue("playercolors","#ff0000,#00ff00,#0000ff,#a77a00").split(",")
        if (scrPlayerColors.count === aColors.length) {
            for (i = 0; i < scrPlayerColors.count; ++i)
                scrPlayerColors.setProperty(i,"itemColor",Qt.color(aColors[i].trim()));
        } else console.warn("Unexpected number of player colors in configuration")

        configBoard.cbPlayerColors.checked = getConfigValue("cbPlayerColors","true") === "true"

        //letters
        configLetter.cbLetterSet.currentIndex = -1
        configLetter.cbLetterSet.currentIndex = getConfigValue("cbLetterSet", 0)
        configLetter.sbJoker.value = getConfigValue("sbJokers", 2)
        configLetter.sbPieces.value = getConfigValue("sbPieces", 7)
        configLetter.sbRandoms.value = getConfigValue("sbRandoms", 0)
        configLetter.rbReadingDirectionLTR.checked = getConfigValue("rbReadingDirectionLTR", "true") === "true"
        configLetter.rbReadingDirectionRTL.checked = getConfigValue("rbReadingDirectionLTR", "true") !== "true"

        //rules
        configRules.rbNoLimit.checked = getConfigValue("rbNoLimit", "false") === "true"
        configRules.rbPerMove.checked = getConfigValue("rbPerMove", "false") === "true"
        configRules.tiPerMove.text = getConfigValue("tiPerMove", "0:01:00")
        configRules.rbPerGame.checked = getConfigValue("rbPerGame", "true") === "true"
        configRules.tiPerGame.text = getConfigValue("tiPerGame", "0:50:00")
        configRules.rbGameEnd.checked = getConfigValue("rbGameEnd", "false") === "true"
        configRules.rbPenalty.checked = getConfigValue("rbPenalty", "true") === "true"
        configRules.sbPenaltyPoints.value = getConfigValue("sbPenaltyPoints", 10)
        configRules.sbPenaltyCount.value = getConfigValue("sbPenaltyCount", 10)
        configRules.cbGameEnd.checked = getConfigValue("cbGameEnd", "true") === "true"
        configRules.sbPasses.value = getConfigValue("sbPasses", 3)

        // dictionary
        var aFileName = getConfigValue("dictionary","")
        if (configDictionary.loadFromName(aFileName)) {
            var aCat = getConfigValue("categories","").split(",")
            if (configDictionary.categoriesRepeater.count == aCat.length+1) {
                for (i = 0; i < aCat.length; ++i)
                    configDictionary.categoriesRepeater.itemAt(i+1).checked = (aCat[i] === "true")
            } else
                console.warn("Unexpected number of category items in configuration")
        }
    }

    ListModel {
        id: scrColors
        dynamicRoles: true
    }
    ListModel {
        id: scrPlayerColors
        dynamicRoles: true
    }

    FileDialog {
        id: fileDialog
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: [qsTr("Scrabble3D configuration (*.ssc)"), qsTr("All files (*)")]
        defaultSuffix: "ssc"
        onAccepted: fileMode == FileDialog.OpenFile
                    ? loadConfig(currentFile)
                    : saveConfig(currentFile)
    }

    Action {
        id: acClose
        text: qsTr("Close")
        icon.source: "qrc:///resources/nextplayer.png"
        onTriggered: config.close()
    }
    Action {
        id: acLoadConfig
        text: qsTr("Load")
        icon.source: "qrc:///resources/load.png"
        onTriggered: {
            fileDialog.acceptLabel = qsTr("Open")
            fileDialog.fileMode = FileDialog.OpenFile
            fileDialog.title = qsTr("Load configuration")
            fileDialog.open()
        }
    }
    Action {
        id: acSaveConfig
        text: qsTr("Save")
        icon.source: "qrc:///resources/save.png"
        onTriggered: {
            fileDialog.acceptLabel = qsTr("Save")
            fileDialog.fileMode = FileDialog.SaveFile
            fileDialog.title = qsTr("Save configuration")
            fileDialog.open()
        }
    }
    Action {
        id: acResetConfig
        text: qsTr("Reset")
        icon.source: "qrc:///resources/reset.png"
        onTriggered: applyConfig({})
    }

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    color: myPalette.base

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12

        SplitView {
            id: splitView

            Layout.fillWidth: true
            Layout.fillHeight: true

            focus: true
            Keys.onEscapePressed: close()

            Rectangle {
                id: leftPane
                SplitView.preferredWidth: parent.width * 1/4
                SplitView.minimumWidth: 50
                color: myPalette.window
                ListView {
                    id: lView
                    anchors.fill: parent
                    anchors.margins: 10
                    Component {
                        id: lViewDelegates
                        Rectangle {
                            width: parent.width
                            height: 32
                            color: ListView.isCurrentItem ? myPalette.highlight : "transparent"
                            Text {
                                id: delegateText
                                text: name
                                leftPadding: 8
                                anchors.verticalCenter: parent.verticalCenter
                                color: lView.currentIndex == index ? myPalette.highlightedText : myPalette.windowText
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: lView.currentIndex = index
                            }
                        }
                    }

                    model: ListModel {
                        ListElement { name: qsTr("Board") }
                        ListElement { name: qsTr("Letters") }
                        ListElement { name: qsTr("Rules") }
                        ListElement { name: qsTr("Dictionary") }
                    }
                    delegate: lViewDelegates
                }
            }

            Rectangle {
                id: rightPane
                SplitView.minimumWidth: 100
                SplitView.fillWidth: true
                color: myPalette.window
                ScrollView {
                    id: scrollView
                    anchors.fill: parent
                    topPadding: 10
                    clip: true
                    ScrConfigBoard {
                        id: configBoard
                        visible: lView.currentIndex == 0
                        onVisibleChanged: { heightChanged() }
                        onHeightChanged: if (visible) {
                            scrollView.contentHeight = configBoard.height
                            scrollView.contentWidth = configBoard.width
                        }
                    }
                    ScrConfigLetter {
                        id: configLetter
                        visible: lView.currentIndex == 1
                        onVisibleChanged: {
                            scrollView.contentHeight = configLetter.height
                            scrollView.contentWidth = configLetter.width
                        }
                    }
                    ScrConfigRules {
                        id: configRules
                        visible: lView.currentIndex == 2
                        defaultLetterSet: configLetter.cbLetterSet.currentIndex > -1
                                          ? configLetter.cbLetterSet.currentIndex
                                          : defaultLetterSet
                        onVisibleChanged: {
                            scrollView.contentHeight = configRules.height
                            scrollView.contentWidth = configRules.width
                        }
                    }
                    ScrConfigDictionary {
                        id: configDictionary
                        visible: lView.currentIndex == 3
                        onVisibleChanged: {
                            scrollView.contentHeight = configDictionary.height + 50 //some space for categories
                            scrollView.contentWidth = configDictionary.width
                        }
                    }
                }
            }
        }

        Rectangle {
            id: configFooter
            height: btnClose.height+12
            Layout.fillWidth: true
            color: "transparent"
            Button {
                id: btnLoad
                action: acLoadConfig
                icon.width: 16; icon.height: 16 //needed on macOS
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }
            Button {
                id: btnSave
                action: acSaveConfig
                icon.width: 16; icon.height: 16
                anchors.left: btnLoad.right
                anchors.leftMargin: 3
                anchors.verticalCenter: parent.verticalCenter
            }
            Button {
                id: btnReset
                action: acResetConfig
                icon.width: 16; icon.height: 16
                display: AbstractButton.IconOnly
                height: btnSave.height
                width: height
                anchors.left: btnSave.right
                anchors.leftMargin: 3
                anchors.verticalCenter: parent.verticalCenter
                ToolTip {
                    text: acResetConfig.text
                    visible: btnReset.hovered
                    delay: 1000
                    timeout: 5000
                }
            }
            Button {
                id: btnClose
                action: acClose
                icon.width: 16; icon.height: 16
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 10
            }
        }

    }

}
