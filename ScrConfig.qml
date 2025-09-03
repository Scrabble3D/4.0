import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform

//Standardpath, native dialog; instead of import QtQuick.Dialogs
Window {
    visible: false //show per action acConfiguration
    modality: Qt.ApplicationModal
    title: qsTr("Configuration")
    width: 680 > screen.width ? screen.width : 680
    height: 540 > screen.height ? screen.height : 540

    property alias showTips: cbShowTips.checked

    property bool bMarkers: true // ConfigBoard::cbMarkers.checked
    property bool bColoredPlayers: true // ConfigBoard::cbPlayerColors.checked
    property bool bIs3D: false // ConfigBoard::rb3D.checked

    property int numberOfLettersOnRack: 7 // ConfigLetter::sbPieces.value
    property int numberOfRandomLetters: 0 // ConfigLetter::sbRandoms.value
    property int numberOfJokers: 2 // ConfigLetter::sbJoker.value
    property bool ltr: true // configLetter::rbReadingDirectionLTR

    property int numberOfPasses: 3 // ConfigTime::sbPasses
    property int timeControl: 0 // ConfigTime::rbNoLimit/rbPerMove/rbPerGame
    property int timeControlValue: 10 // ConfigTime::rbNoLimit/rbPerMove/rbPerGame

    property int bingoBonus: 50 // ConfigRules::sbBingo
    property int gameEndBonus: 0 // ConfigRules::sbGameEnd
    property bool addLetters: true // ConfigRules::sbGameEnd
    property bool substractLetters: true // ConfigRules::sbGameEnd
    property int jokerPenalty: 0 // ConfigRules::sbJokerPanelty
    property bool changeIsPass: false // ConfigRules::cbChangeIsPass
    property bool cambiosecco: false // ConfigRules::cbCambioSecco
    property int performance: 0 // ConfigRules::slPerformance

    property int wordCheckMode: 0 // ConfigWordCheck::rbTakeback = 0, rbPoll = 1, rbChallenge = 2
    property int wordCheckPeriod: 10 // ConfigWordCheck::sbPeriod.value
    property int wordCheckPenalty: 10 // ConfigWordCheck::sbPenalty.value
    property int wordCheckBonus: 0 // ConfigWordCheck::sbBonus.value
    property bool clabbers: false // ConfigWordCheck::cbClabbers

    property bool canbuytime: false // ConfigTimeControl::rbGameEnd/rbPenalty
    property int buytimecount: 0 // ConfigTimeControl::sbPenaltyCount
    property int buytimepenalty: 0 // ConfigTimeControl::sbPenaltyPoints
    property bool timegamelost: false // ConfigTimeControl::sbPenaltyPoints

    property alias colors: scrColors
    property alias playercolors: scrPlayerColors
    property var board: []

    property alias myPalette: myPalette

    property string boardName: configBoard.boardName
    property string lettersetName: configLetter.getLetterSetName()
    property string dictionaryName: configDictionary.dictionaryName
    Component.onCompleted: {
        //fill color names with random color values and reset later to the default
        var cNames = [qsTr("Start"), qsTr("Default"), qsTr("2x letter"), qsTr(
                          "3x letter"), qsTr("4x letter"), qsTr(
                          "2x word"), qsTr("3x word"), qsTr("4x word"), qsTr(
                          "Pieces")]
        for (var i = 0; i < cNames.length; i++)
            scrColors.append({
                                 "itemColor": "#000000",
                                 "itemName": cNames[i]
                             })
        cNames = [qsTr("First player"), qsTr("Second player"), qsTr(
                      "Third player"), qsTr("Fourth player")]
        for (i = 0; i < cNames.length; i++)
            scrPlayerColors.append({
                                       "itemColor": "#000000",
                                       "itemName": cNames[i]
                                   })

        //load last session properties
        loadConfig("")
    }
    function getLetterSet(index) {
        return configLetter.getLetterSet(index)
    }

    function getConfigData(bComplete = true) {
        var configData = {}
        //board
        if (bComplete)
            configData["cbMarkers"] = configBoard.cbMarkers.checked
        configData["rb3D"] = configBoard.rb3D.checked
        configData["board"] = board.toString()
        if (bComplete) {
            var aColors = []
            for (var i = 0; i < scrColors.count; ++i)
                aColors.push(scrColors.get(i).itemColor.toString())
            configData["colors"] = aColors.toString()
            aColors = []
            for (i = 0; i < scrPlayerColors.count; ++i)
                aColors.push(scrPlayerColors.get(i).itemColor.toString())
            configData["playercolors"] = aColors.toString()
            configData["cbPlayerColors"] = configBoard.cbPlayerColors.checked
        }
        //letter
        configData["cbLetterSet"] = configLetter.cbLetterSet.currentIndex
        if (configLetter.cbLetterSet.currentIndex === -1)
            configData["cbLetterSetData"] = getLetterSet(-1).toString()
        configData["sbJokers"] = configLetter.sbJoker.value
        configData["sbPieces"] = configLetter.sbPieces.value
        configData["sbRandoms"] = configLetter.sbRandoms.value
        if (bComplete)
            configData["rbReadingDirectionLTR"] = configLetter.rbReadingDirectionLTR.checked
        //configWordCheck
        configData["rbTakeback"] = configWordCheck.rbTakeback.checked
        configData["rbPoll"] = configWordCheck.rbPoll.checked
        configData["rbChallenge"] = configWordCheck.rbChallenge.checked
        configData["sbPeriod"] = configWordCheck.sbPeriod.value
        configData["sbPenalty"] = configWordCheck.sbPenalty.value
        configData["sbBonus"] = configWordCheck.sbBonus.value
        configData["cbClabbers"] = configWordCheck.cbClabbers.checked
        //configTime
        configData["rbNoLimit"] = configTime.rbNoLimit.checked
        configData["rbPerMove"] = configTime.rbPerMove.checked
        configData["tiPerMove"] = configTime.tiPerMove.text
        configData["rbPerGame"] = configTime.rbPerGame.checked
        configData["tiPerGame"] = configTime.tiPerGame.text
        configData["rbGameEnd"] = configTime.rbGameEnd.checked
        configData["rbPenalty"] = configTime.rbPenalty.checked
        configData["sbPenaltyPoints"] = configTime.sbPenaltyPoints.value
        configData["sbPenaltyCount"] = configTime.sbPenaltyCount.value
        configData["cbGameEnd"] = configTime.cbGameEnd.checked
        //configRules
        configData["sbPasses"] = configRules.sbPasses.value
        configData["sbBingo"] = configRules.sbBingo.value
        configData["sbGameEnd"] = configRules.sbGameEnd.value
        configData["sbJokerPenalty"] = configRules.sbJokerPenalty.value
        configData["cbAddLetters"] = configRules.cbAddLetters.checked
        configData["cbSubstractLetters"] = configRules.cbSubstractLetters.checked
        configData["cbChangeIsPass"] = configRules.cbChangeIsPass.checked
        configData["cbCambioSecco"] = configRules.cbCambioSecco.checked
        configData["slPerformance"] = configRules.slPerformance.value

        //dictionary
        configData["dictionary"] = configDictionary.dictionaryFile
        const dictionaryCategories = configDictionary.dictionaryCategories
        var selectedCategories = []
        for (i = 0; i < dictionaryCategories.length; i++)
            if (GamePlay.getCategoryChecked(dictionaryCategories[i]))
                selectedCategories.push(dictionaryCategories[i])
        configData["categories"] = selectedCategories.toString()
        return configData
    }
    function saveConfig(fileName) {
        GamePlay.saveConfig(fileName, getConfigData())
    }
    function loadConfig(fileName) {
        //reset calls applyConfig({})
        applyConfig(GamePlay.loadConfig(fileName))
    }
    function applyConfig(configData) {
        function getConfigValue(key, def) {
            return configData[key] || def
        }

        //board
        configBoard.cbMarkers.checked = getConfigValue("cbMarkers",
                                                       "true") === "true"
        if (getConfigValue("rb3D", "false") === "true")
            configBoard.rb3D.checked = true
        else
            configBoard.rb2D.checked = true
        board = []
        board = getConfigValue(
                    "board",
                    "6,1,1,2,1,1,1,6,1,1,1,2,1,1,6,1,5,1,1,1,3,1,1,1,3,1,1,1,5,1,1,1,5,1,1,1,2,1,2,1,1,1,5,1,1,2,1,1,5,1,1,1,2,1,1,1,5,1,1,2,1,1,1,1,5,1,1,1,1,1,5,1,1,1,1,1,3,1,1,1,3,1,1,1,3,1,1,1,3,1,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,6,1,1,2,1,1,1,0,1,1,1,2,1,1,6,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,3,1,1,1,3,1,1,1,3,1,1,1,3,1,1,1,1,1,5,1,1,1,1,1,5,1,1,1,1,2,1,1,5,1,1,1,2,1,1,1,5,1,1,2,1,1,5,1,1,1,2,1,2,1,1,1,5,1,1,1,5,1,1,1,3,1,1,1,3,1,1,1,5,1,6,1,1,2,1,1,1,6,1,1,1,2,1,1,6").split(
                    ",").map(function (t) {
                        return parseInt(t)
                    })
        configBoard.checkDefault()
        if (configBoard.rb2D.checked)
            configBoard.sbSize.value = Math.sqrt(config.board.length)
        else
            configBoard.sbSize.value = Math.round(Math.cbrt(
                                                      config.board.length))
        var aColors = getConfigValue("colors", "#c0c0c0,#008000,#00ffff,
#0000ff,#000060,#ff6060,
#ff0000,#600000,#ffff00").split(",")
        if (scrColors.count === aColors.length) {
            for (var i = 0; i < scrColors.count; ++i)
                scrColors.setProperty(i, "itemColor",
                                      Qt.color(aColors[i].trim()))
        } else
            console.warn("Unexpected number of colors in configuration")

        aColors = getConfigValue("playercolors",
                                 "#ff0000,#00ff00,#0000ff,#a77a00").split(",")
        if (scrPlayerColors.count === aColors.length) {
            for (i = 0; i < scrPlayerColors.count; ++i)
                scrPlayerColors.setProperty(i, "itemColor",
                                            Qt.color(aColors[i].trim()))
        } else
            console.warn("Unexpected number of player colors in configuration")

        configBoard.cbPlayerColors.checked = getConfigValue("cbPlayerColors",
                                                            "true") === "true"

        //letters
        configLetter.cbLetterSet.currentIndex = -1
        configLetter.cbLetterSet.currentIndex = getConfigValue("cbLetterSet", 0)
        //load letter set before dictionary to suppress warning of different letter sets
        if (configLetter.cbLetterSet.currentIndex === -1) {
            var aLetters = []
            aLetters = getConfigValue("cbLetterSetData", "").split(",")
            if (aLetters.length > 0)
                configLetter.setLetterSet(aLetters)
        }

        configLetter.sbJoker.value = getConfigValue("sbJokers", 2)
        configLetter.sbPieces.value = getConfigValue("sbPieces", 7)
        configLetter.sbRandoms.value = getConfigValue("sbRandoms", 0)
        configLetter.rbReadingDirectionLTR.checked = getConfigValue(
                    "rbReadingDirectionLTR", "true") === "true"
        configLetter.rbReadingDirectionRTL.checked = getConfigValue(
                    "rbReadingDirectionLTR", "true") !== "true"

        //configWordCheck
        configWordCheck.rbTakeback.checked = getConfigValue("rbTakeback",
                                                            "true") === "true"
        configWordCheck.rbPoll.checked = getConfigValue("rbPoll",
                                                        "false") === "true"
        configWordCheck.rbChallenge.checked = getConfigValue("rbChallenge",
                                                             "false") === "true"
        configWordCheck.sbPeriod.value = getConfigValue("sbPeriod", 10)
        configWordCheck.sbPenalty.value = getConfigValue("sbPenalty", 10)
        configWordCheck.sbBonus.value = getConfigValue("sbBonus", 0)
        configWordCheck.cbClabbers.checked = getConfigValue("cbClabbers",
                                                            "false") === "true"

        //time
        configTime.rbNoLimit.checked = getConfigValue("rbNoLimit",
                                                      "false") === "true"
        configTime.rbPerMove.checked = getConfigValue("rbPerMove",
                                                      "false") === "true"
        configTime.tiPerMove.text = getConfigValue("tiPerMove", "0:01:00")
        configTime.rbPerGame.checked = getConfigValue("rbPerGame",
                                                      "true") === "true"
        configTime.tiPerGame.text = getConfigValue("tiPerGame", "0:50:00")
        configTime.rbGameEnd.checked = getConfigValue("rbGameEnd",
                                                      "false") === "true"
        configTime.rbPenalty.checked = getConfigValue("rbPenalty",
                                                      "true") === "true"
        configTime.sbPenaltyPoints.value = getConfigValue("sbPenaltyPoints", 10)
        configTime.sbPenaltyCount.value = getConfigValue("sbPenaltyCount", 10)
        configTime.cbGameEnd.checked = getConfigValue("cbGameEnd",
                                                      "true") === "true"

        //rules
        configRules.sbPasses.value = getConfigValue("sbPasses", 3)
        configRules.sbBingo.value = getConfigValue("sbBingo", 50)
        configRules.sbGameEnd.value = getConfigValue("sbGameEnd", 0)
        configRules.sbJokerPenalty.value = getConfigValue("sbJokerPenalty", 0)
        configRules.cbAddLetters.checked = getConfigValue("cbAddLetters",
                                                          "true") === "true"
        configRules.cbSubstractLetters.checked = getConfigValue(
                    "cbSubstractLetters", "true") === "true"
        configRules.cbChangeIsPass.checked = getConfigValue("cbChangeIsPass",
                                                            "false") === "true"
        configRules.cbCambioSecco.checked = getConfigValue("cbCambioSecco",
                                                           "false") === "true"
        configRules.slPerformance.value = getConfigValue("slPerformance", 10)

        // dictionary
        var aFileName = getConfigValue("dictionary", "")
        var aCategories = getConfigValue("categories", "")
        if (aFileName !== configDictionary.dictionaryFile)
            GamePlay.loadDictionary(aFileName, aCategories)
        else
            configDictionary.setCategories(
                        aCategories) // same dictionary name but different categories in network mode
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
        nameFilters: [qsTr("Scrabble3D configuration (*.ssc)"), qsTr(
                "All files (*)")]
        defaultSuffix: "ssc"
        onAccepted: {
            fileMode === FileDialog.OpenFile ? loadConfig(
                                                   file) : saveConfig(file)
        }
    }

    Action {
        id: acClose
        text: qsTr("Close")
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

    SystemPalette {
        id: aPalette
        colorGroup: SystemPalette.Active
    }
    SystemPalette {
        id: bPalette
        colorGroup: SystemPalette.Disabled
    }
    Palette {
        id: myPalette
        alternateBase: aPalette.alternateBase
        base: aPalette.base
        button: aPalette.button
        buttonText: aPalette.buttonText
        dark: aPalette.dark
        highlight: aPalette.highlighted
        highlightedText: aPalette.highlightedText
        light: aPalette.light
        mid: aPalette.mid
        midlight: aPalette.midlight
        placeholderText: aPalette.placeholderText
        shadow: aPalette.shadow
        text: aPalette.text
        window: aPalette.window
        windowText: aPalette.windowText

        disabled.alternateBase: bPalette.alternateBase
        disabled.base: bPalette.base
        disabled.button: bPalette.button
        disabled.buttonText: bPalette.buttonText
        disabled.dark: bPalette.dark
        disabled.highlight: bPalette.highlighted
        disabled.highlightedText: bPalette.highlightedText
        disabled.light: bPalette.light
        disabled.mid: bPalette.mid
        disabled.midlight: bPalette.midlight
        disabled.placeholderText: bPalette.placeholderText
        disabled.shadow: bPalette.shadow
        disabled.text: bPalette.text
        disabled.window: bPalette.window
        disabled.windowText: bPalette.windowText
    }
    color: myPalette.base

    ListModel {
        id: lmCategories
        ListElement {
            name: qsTr("Board")
            imgname: "optboard.png"
        }
        ListElement {
            name: qsTr("Letters")
            imgname: "optletters.png"
        }
        ListElement {
            name: qsTr("Word Check Mode")
            imgname: "optwordcheck.png"
        }
        ListElement {
            name: qsTr("Time Control")
            imgname: "opttime.png"
        }
        ListElement {
            name: qsTr("Rules")
            imgname: "optrules.png"
        }
        ListElement {
            name: qsTr("Dictionary")
            imgname: "optdic.png"
        }
        ListElement {
            name: qsTr("Localization")
            imgname: "optui.png"
        }
    }

    Component {
        id: lViewDelegates
        Rectangle {
            width: parent.width
            height: 48 + 12
            color: ListView.isCurrentItem ? myPalette.highlight : "transparent"
            Image {
                id: delegateImage
                source: "qrc:///resources/" + imgname
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                id: delegateText
                text: name
                leftPadding: 8
                anchors.left: delegateImage.right
                anchors.verticalCenter: parent.verticalCenter
                color: lView.currentIndex
                       === index ? myPalette.highlightedText : myPalette.windowText
            }
            MouseArea {
                anchors.fill: parent
                onClicked: lView.currentIndex = index
            }
        }
    }

    ColumnLayout {
        anchors {
            fill: parent

            // Android system bars
            topMargin: parent.SafeArea.margins.top
            leftMargin: parent.SafeArea.margins.left
            rightMargin: parent.SafeArea.margins.right
            bottomMargin: parent.SafeArea.margins.bottom
        }

        SplitView {
            id: splitView

            //hide the handle left-hand of the splitview
            handle: Rectangle {
                visible: mainLoader.state === "landscape" && x > 10
                implicitWidth: 2
                implicitHeight: 2
                color: SplitHandle.pressed ? Qt.lighter(
                                                 palette.dark,
                                                 1.5) : (SplitHandle.hovered ? Qt.lighter(palette.dark, 1.1) : palette.dark)

                containmentMask: Item {
                    x: -width / 2
                    width: 48
                    height: splitView.height
                }
            }

            orientation: mainLoader.state === "landscape" ? Qt.Horizontal : Qt.Vertical

            Layout.fillWidth: true
            Layout.fillHeight: true

            focus: true
            Keys.onEscapePressed: close()

            Rectangle {
                id: leftPane
                visible: mainLoader.state === "landscape"
                SplitView.preferredWidth: parent.width * 28 / 100
                SplitView.minimumWidth: 50
                color: myPalette.window //"#FF0000" //
                ListView {
                    id: lView
                    anchors.fill: parent
                    model: lmCategories
                    delegate: lViewDelegates
                }
            }
            ColorComboBox {
                id: catSelector
                visible: mainLoader.state === "portrait"
                model: lmCategories
                textRole: "name"
                onCurrentIndexChanged: lView.currentIndex = currentIndex
            }
            Rectangle {
                id: rightPane
                SplitView.minimumWidth: 100
                SplitView.fillWidth: true
                color: myPalette.window
                ScrollView {
                    id: scrollView
                    anchors.fill: parent
                    ConfigBoard {
                        id: configBoard
                        visible: lView.currentIndex === 0
                        onVisibleChanged: {
                            scrollView.contentHeight = configBoard.height
                            scrollView.contentWidth = configBoard.width
                        }
                    }
                    ConfigLetter {
                        id: configLetter
                        visible: lView.currentIndex === 1
                        onVisibleChanged: {
                            scrollView.contentHeight = configLetter.height
                            scrollView.contentWidth = configLetter.width
                        }
                    }
                    ConfigWordCheck {
                        id: configWordCheck
                        visible: lView.currentIndex === 2
                        defaultLetterSet: configLetter.cbLetterSet.currentIndex
                                          > -1 ? configLetter.cbLetterSet.currentIndex : defaultLetterSet
                        onVisibleChanged: {
                            scrollView.contentHeight = configWordCheck.height
                            scrollView.contentWidth = configWordCheck.width
                        }
                    }
                    ConfigTime {
                        id: configTime
                        visible: lView.currentIndex === 3
                        defaultLetterSet: configLetter.cbLetterSet.currentIndex
                                          > -1 ? configLetter.cbLetterSet.currentIndex : defaultLetterSet
                        onVisibleChanged: {
                            scrollView.contentHeight = configTime.height
                            scrollView.contentWidth = configTime.width
                        }
                    }
                    ConfigRules {
                        id: configRules
                        visible: lView.currentIndex === 4
                        defaultLetterSet: configLetter.cbLetterSet.currentIndex
                                          > -1 ? configLetter.cbLetterSet.currentIndex : defaultLetterSet
                        onVisibleChanged: {
                            scrollView.contentHeight = configRules.height
                            scrollView.contentWidth = configRules.width
                        }
                    }
                }
                ConfigDictionary {
                    id: configDictionary
                    visible: lView.currentIndex === 5
                }
                ConfigUI {
                    id: configUI
                    visible: lView.currentIndex === 6
                }
            }
        }

        Rectangle {
            id: configFooter
            Layout.minimumHeight: btnClose.height + 12
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.bottomMargin: 6
            color: "transparent"
            ColorButton {
                id: btnLoad
                leftPadding: 8
                rightPadding: 8
                spacing: 2
                action: acLoadConfig
                display: config.width
                         < 500 ? AbstractButton.IconOnly : AbstractButton.TextBesideIcon
                width: display == AbstractButton.IconOnly ? height : Math.max(
                                                                implicitBackgroundWidth + leftInset
                                                                + rightInset,
                                                                implicitContentWidth + leftPadding
                                                                + rightPadding)
                icon.width: 16
                icon.height: 16 //needed on macOS
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
            }
            ColorButton {
                id: btnSave
                leftPadding: 8
                rightPadding: 8
                spacing: 2
                action: acSaveConfig
                display: config.width
                         < 500 ? AbstractButton.IconOnly : AbstractButton.TextBesideIcon
                width: display == AbstractButton.IconOnly ? height : Math.max(
                                                                implicitBackgroundWidth + leftInset
                                                                + rightInset,
                                                                implicitContentWidth + leftPadding
                                                                + rightPadding)
                icon.width: 16
                icon.height: 16
                anchors.left: btnLoad.right
                anchors.leftMargin: 3
                anchors.verticalCenter: parent.verticalCenter
            }
            ColorButton {
                id: btnReset
                action: acResetConfig
                icon.width: 16
                icon.height: 16
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
            ColorCheckBox {
                id: cbShowTips
                anchors.left: btnReset.right
                anchors.leftMargin: 3
                anchors.verticalCenter: parent.verticalCenter
                checked: true
                text: qsTr("Show tips")
            }
            ColorButton {
                id: btnClose
                leftPadding: 8
                rightPadding: 8
                spacing: 2
                action: acClose
                icon.width: 16
                icon.height: 16
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 10
            }
        }
    }
}
