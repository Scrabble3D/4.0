import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: newgame
    //: dialog title
    title: qsTr("Start a new game")
    modal: true
    footer: DialogButtonBox {
        id: buttons
        standardButtons: Dialog.Ok | Dialog.Cancel
    }
    property alias playerNames: playerNames //used at poll
    property bool isMobileUI: mainLoader.state === "landscape"
    padding: isMobileUI ? 12 : 3

    x: (scrabble3D.width - width) / 2
    y: (scrabble3D.height - height) / 2
    height: Math.max(pnLeft.height, pnRight.height) + implicitFooterHeight + implicitHeaderHeight + padding
    width: configInfo.width + 2 * padding

    property int rbIndex: 1
    property int seed: -1

    function setPlayers(configData) {
        playerNames.clear()
        rbIndex = configData["playerCount"] || 1
        playerNames.append({"playerName": configData["player1"] || qsTr("1st Player"),
                               "isChecked": rbIndex === 1,
                               "isComputer": false})
        playerNames.append({"playerName": configData["player2"] || qsTr("2nd Player"),
                               "isChecked": rbIndex === 2,
                               "isComputer": false})
        playerNames.append({"playerName": configData["player3"] || qsTr("3rd Player"),
                               "isChecked": rbIndex === 3,
                               "isComputer": false})
        playerNames.append({"playerName": configData["player4"] || qsTr("4th Player"),
                               "isChecked": rbIndex === 4,
                               "isComputer": false})
        seed = configData["seed"] || -1
        cbRandomized.checked = true
    }

    ListModel {
        id: playerNames
        dynamicRoles: true
    }
    onAboutToShow: {
        if (!GamePlay.isConnected) {
            var configData = {}
            configData = GamePlay.loadConfig("")
            rbIndex = configData["playerCount"] || 1
            playerNames.clear()
            if (GamePlay.isDemo) {
                playerNames.append({"playerName": qsTr("1st Player"),
                                       "isChecked": rbIndex === 1,
                                       "isComputer": false})
                playerNames.append({"playerName": qsTr("2nd Player"),
                                       "isChecked": rbIndex === 2,
                                       "isComputer": false})
                playerNames.append({"playerName": qsTr("3rd Player"),
                                       "isChecked": rbIndex === 3,
                                       "isComputer": false})
                playerNames.append({"playerName": qsTr("4th Player"),
                                       "isChecked": rbIndex === 4,
                                       "isComputer": false})
            } else
            {
                playerNames.append({"playerName": configData["player1"] || qsTr("1st Player"),
                                       "isChecked": rbIndex === 1,
                                       "isComputer": false})
                playerNames.append({"playerName": configData["player2"] || qsTr("2nd Player"),
                                       "isChecked": rbIndex === 2,
                                       "isComputer": configData["comp2"] === "true" || false})
                playerNames.append({"playerName": configData["player3"] || qsTr("3rd Player"),
                                       "isChecked": rbIndex === 3,
                                       "isComputer": configData["comp3"] === "true" || false})
                playerNames.append({"playerName": configData["player4"] || qsTr("4th Player"),
                                       "isChecked": rbIndex === 4,
                                       "isComputer": configData["comp4"] === "true" || false})
            }
            cbRandomized.checked = configData["randomized"] === "true" || false
        }
        buttons.enabled = true //disabled on okay while waiting on network poll
        newgame.focus = true
    }

    function getTimeString() {
        var aTime = new Date(0,0,0,0,0,0)
        aTime.setSeconds(config.timeControlValue)
        return aTime.toTimeString()
    }
    contentItem: FocusScope {
        id: focus //required to receive return key
        Keys.onReturnPressed: accept()
        focus: standardButton(Dialog.Ok)
        ButtonGroup { id: radioGroup }
        Component {
            id: playerDelegate
            Row {
                id: layout
                spacing: 4
                RadioButton {
                    id: rbPlayerCount
                    text: (index + 1)
                    checked: isChecked
                    enabled: !GamePlay.isConnected
                    onCheckedChanged: rbIndex = index + 1
                    ButtonGroup.group: radioGroup
                    //todo: newgame: indicator of radiobutton not padded under windows
                    topPadding: tiName.topPadding
                }
                Column {
                    spacing: 1
                    TextField   {
                        id: tiName
                        width: 120
                        bottomPadding: topPadding
                        text: playerName
                        enabled: (rbIndex > index) && (!cbComputer.checked)
                        readOnly: GamePlay.isConnected
                        background: Rectangle {
                            color: tiName.enabled
                                   ? "transparent"
                                   : config.myPalette.mid
                            border.color: tiName.enabled
                                          ? config.myPalette.mid
                                          : config.myPalette.midlight
                        }
                        leftPadding: 2
                        verticalAlignment: TextInput.AlignVCenter //Qt.AlignVCenter
                        mouseSelectionMode: TextInput.SelectWords
                        selectByMouse: true
                        onFocusChanged: if (focus) selectAll()
                        onEditingFinished: if (text !== "Computer")
                                               playerNames.setProperty(index,"playerName",text)
                    }
                    CheckBox {
                        id: cbComputer
                        text: "Computer"
                        checked: isComputer
                        topPadding: tiName.topPadding
                        leftPadding: 0
                        onClicked: playerNames.setProperty(index,"isComputer",checked)
                        visible: index > 0 //at least one human player should be in the game
                        enabled: (rbIndex > index) && (!GamePlay.isConnected)
                    }
                }
            }
        }
        RowLayout {
            id: configInfo
            spacing: 12
            ColumnLayout {
                id: pnLeft
                Layout.alignment: Qt.AlignTop
                RowLayout {
                    id: header
                    Label  {
                        leftPadding: 25 //~ width(radiobutton) + spacing
                        // hash symbol indicating the number of players in the new_game-dialog
                        text: "#"
                        font.bold: true
                    }
                    Label {
                        text: qsTr("player name")
                        font.bold: true
                    }
                }
                Repeater {
                    model: playerNames
                    delegate: playerDelegate
                }
                CheckBox {
                    id: cbRandomized
                    text: "Randomized order"
                    focus: false
                    enabled: !GamePlay.isConnected
                }
            }
            GridLayout {
                id: pnRight
                columns: 2
                rowSpacing: 6
                Layout.alignment: Qt.AlignTop
                Label { Layout.columnSpan: 2; text: qsTr("Settings:"); font.bold: true }
                Label { text: qsTr("Board:"); Layout.columnSpan: isMobileUI ? 1 : 2}
                Label { text: config.boardName; Layout.columnSpan: isMobileUI ? 1 : 2; leftPadding: isMobileUI ? 0 : 12 }
                Label { text: qsTr("Letters:"); Layout.columnSpan: isMobileUI ? 1 : 2 }
                Label { text: config.lettersetName; Layout.columnSpan: isMobileUI ? 1 : 2; leftPadding: isMobileUI ? 0 : 12  }
                Label { text: qsTr("Dictionary:"); Layout.columnSpan: isMobileUI ? 1 : 2 }
                Label { text: config.dictionaryName; Layout.columnSpan: isMobileUI ? 1 : 2; leftPadding: isMobileUI ? 0 : 12  }
                Label { text: qsTr("Time control:"); Layout.columnSpan: isMobileUI ? 1 : 2 }
                Label { text: config.timeControl == 0
                              ? qsTr("No time control")
                              : config.timeControl == 1
                                ? qsTr("Per Move") + " (" + getTimeString() + ")"
                                : qsTr("Per Game") + " (" + getTimeString() + ")"
                        Layout.columnSpan: isMobileUI ? 1 : 2; leftPadding: isMobileUI ? 0 : 12 }
                Label { text: qsTr("Word check mode:"); Layout.columnSpan: isMobileUI ? 1 : 2 }
                Label { text: config.wordCheckMode == 0
                              ? qsTr("Takeback")
                              : config.wordCheckMode == 1
                                ? qsTr("Poll")
                                : qsTr("Challenge") + " (" +
                                  //: unit seconds
                                  config.wordCheckPeriod + qsTr("s") + ")";
                        Layout.columnSpan: isMobileUI ? 1 : 2; leftPadding: isMobileUI ? 0 : 12 }
                Label { text: qsTr("Cambio Secco:"); visible: config.cambiosecco; Layout.columnSpan: isMobileUI ? 1 : 2 }
                Label { text: qsTr("enabled"); visible: config.cambiosecco; Layout.columnSpan: isMobileUI ? 1 : 2; leftPadding: isMobileUI ? 0 : 12  }
            }
        }
    }

    function getNames() {
        let sRet = []
        for (var i = 0; i < rbIndex; i++)
            if (playerNames.get(i).isComputer)
                sRet.push("Computer")
            else
                sRet.push(playerNames.get(i).playerName)
        return sRet
    }
    onAccepted: {
        buttons.enabled = false //disabled as feedback while poll is active in network mode
        var gameConfig = {}
        gameConfig["PlayerNames"] = getNames()
        gameConfig["RackSize"] = config.numberOfLettersOnRack
        gameConfig["Is3D"] = config.bIs3D
        gameConfig["FieldTypeArray"] = config.board
        gameConfig["LetterList"] = config.getLetterSet(-1) //Letters[letter,value,count]
        gameConfig["NumberOfRandoms"] = config.numberOfRandomLetters
        gameConfig["NumberOfJokers"] = config.numberOfJokers
        gameConfig["CanJokerExchange"] = true //FEATURE: newgame CanJokerExchange
        gameConfig["GameEndBonus"] = config.gameEndBonus
        gameConfig["NumberOfPasses"] = config.numberOfPasses
        gameConfig["JokerPenalty"] = config.jokerPenalty
        gameConfig["ChangeIsPass"] = config.changeIsPass
        gameConfig["TimeControlType"] = config.timeControl // tcNoLimit=0, tcPerMove=1, tcPerGame=2
        gameConfig["TimeControlValue"] = config.timeControlValue
        gameConfig["LimitedExchange"] = 3 //FEATURE: newgame LimitedExchange
        gameConfig["CambioSecco"] = config.cambiosecco
        gameConfig["Whatif"] = false //FEATURE: newgame Whatif
        gameConfig["Add"] = config.addLetters
        gameConfig["Substract"] = config.substractLetters
        gameConfig["TimePenaltyValue"] = config.buytimepenalty
        gameConfig["TimePenaltyCount"] = config.canbuytime ? config.buytimecount : 0
        gameConfig["TimeGameLost"] = config.timegamelost
        gameConfig["WordCheckType"] = config.wordCheckMode
        gameConfig["WordCheckPeriod"] =config.wordCheckPeriod
        gameConfig["WordCheckPenalty"] = config.wordCheckPenalty
        gameConfig["WordCheckBonus"] = config.wordCheckBonus
        gameConfig["ScrabbleBonus"] = config.bingoBonus
        gameConfig["IsCLABBERS"] = config.clabbers
        gameConfig["RandomSequence"] = cbRandomized.checked // -1 = generate
        gameConfig["Seed"] = seed
        gameConfig["Performance"] = config.performance

        if (!GamePlay.isConnected)
            GamePlay.startNewGame(gameConfig)
        else
            GamePlay.startNewGameLater(gameConfig)

        main.board.jokerPicker.updatePickerModel() //use current letterlist for joker picker
        main.board.updateLabelsModel() //redraw labels on x/y axis
        main.board.updateFieldSize() //changing the number of fields should result in resizing

        //save last game settings except when in network mode
        if (!GamePlay.isConnected) {
            var configData = {}
            configData["player1"] = playerNames.get(0).playerName
            configData["player2"] = playerNames.get(1).playerName
            configData["player3"] = playerNames.get(2).playerName
            configData["player4"] = playerNames.get(3).playerName
            configData["comp1"] = playerNames.get(0).isComputer
            configData["comp2"] = playerNames.get(1).isComputer
            configData["comp3"] = playerNames.get(2).isComputer
            configData["comp4"] = playerNames.get(3).isComputer
            configData["playerCount"] = rbIndex
            configData["randomized"] = cbRandomized.checked
            GamePlay.saveConfig("", configData) // player info only
            config.saveConfig("") //all other config
        }

    }
    onRejected: GamePlay.rejectNewGame()
}
