import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    columns: 2
    columnSpacing: 8
    rowSpacing: 8

    property int defaultLetterSet: 0
    onDefaultLetterSetChanged: {
        sbPasses.value = defaults.languages[defaultLetterSet].numberOfPasses
        sbBingo.value = defaults.languages[defaultLetterSet].scrabbleBonus
        sbGameEnd.value = defaults.languages[defaultLetterSet].endBonus
        cbAddLetters.checked = defaults.languages[defaultLetterSet].addLetters
        cbSubstractLetters.checked = defaults.languages[defaultLetterSet].substractLetters
        sbJokerPenalty.value = defaults.languages[defaultLetterSet].jokerPenalty
        cbChangeIsPass.checked = defaults.languages[defaultLetterSet].changeIsPass
        cbCambioSecco.checked = defaults.languages[defaultLetterSet].cambioSecco
    }

    property alias sbPasses: sbPasses
    property alias sbBingo: sbBingo
    property alias sbGameEnd: sbGameEnd
    property alias cbAddLetters: cbAddLetters
    property alias cbSubstractLetters: cbSubstractLetters
    property alias sbJokerPenalty: sbJokerPenalty
    property alias cbChangeIsPass: cbChangeIsPass
    property alias cbCambioSecco: cbCambioSecco
    property alias slPerformance: slPerformance

    ColorLabel {
        id: lbPasses
        Layout.leftMargin: 8
        Layout.topMargin: 8
        Layout.alignment: Qt.AlignRight
        text: qsTr("Passes until game end:")
    }
    RowLayout {
        Layout.topMargin: 8
        ColorSpinBox {
            id: sbPasses
            from: 1
            to: 99
            value: 3
            onValueChanged: config.numberOfPasses = value
        }
        InfoTip { tiptext: qsTr("The number of moves each player can pass consecutively until the game ends") }
    }

    ColorLabel {
        id: lbBingo
        Layout.leftMargin: 8
        Layout.alignment: Qt.AlignRight
        text: qsTr("Bonus for Scrabble/Bingos:")
    }
    RowLayout {
        ColorSpinBox {
            id: sbBingo
            from: 0
            to: 999
            value: 50
            onValueChanged: config.bingoBonus = value
        }
        InfoTip { tiptext: qsTr("The 'bingo' bonus when all tiles from the rack are placed at once") }
    }

    ColorLabel {
        id: lbGameEnd
        Layout.leftMargin: 8
        Layout.alignment: Qt.AlignRight
        text: qsTr("Bonus on game end:")
    }
    RowLayout {
        ColorSpinBox {
            id: sbGameEnd
            from: 0
            to: 999
            value: 0
            onValueChanged: config.gameEndBonus = value
        }
        InfoTip { tiptext: qsTr("Extra bonus awarded to the player who ends the game") }
    }

    ColorLabel {
        id: lbJokerPenalty
        Layout.leftMargin: 8
        Layout.alignment: Qt.AlignRight
        text: qsTr("Penalty for left blanks:")
    }
    RowLayout {
        ColorSpinBox {
            id: sbJokerPenalty
            from: 0
            to: 999
            value: 0
            onValueChanged: config.jokerPenalty = value
        }
        InfoTip { tiptext: qsTr("Extra penalty for blank tile on the rack when the game ends") }
    }

    RowLayout {
        Layout.columnSpan: 2
        Layout.leftMargin: 8
        ColorCheckBox {
            id: cbAddLetters
            text: qsTr("Add the value of other players letters")
            onCheckStateChanged: config.addLetters = checked
        }
        InfoTip { tiptext: qsTr("The sum of remaining tiles from the opponents is added to the result of the player who ended the game") }
    }
    RowLayout {
        Layout.columnSpan: 2
        Layout.leftMargin: 8
        ColorCheckBox {
            id: cbSubstractLetters
            text: qsTr("Substract the value of left letters")
            onCheckStateChanged: config.substractLetters = checked
        }
        InfoTip { tiptext: qsTr("The value of the remaining tiles is deducted from each player's result") }
    }
    RowLayout {
        Layout.columnSpan: 2
        Layout.leftMargin: 8
        ColorCheckBox {
            id: cbChangeIsPass
            text: qsTr("Treat exchange as pass")
            onCheckStateChanged: config.changeIsPass = checked
        }
        InfoTip { tiptext: qsTr("Check this option to count an exchange of letters as a pass") }
    }
    RowLayout {
        Layout.columnSpan: 2
        Layout.leftMargin: 8
        ColorCheckBox {
            id: cbCambioSecco
            text: qsTr("Cambio Secco")
            onCheckStateChanged: config.cambiosecco = checked
        }
        InfoTip { tiptext: qsTr("Once per game you can exchange all pieces on the rack without losing the right to place") }
    }

    ColorLabel {
        Layout.leftMargin: 8
        Layout.topMargin: 16
        text: qsTr("Computer performance:")
    }
    RowLayout {
        Layout.columnSpan: 2
        Layout.leftMargin: 16
        Slider {
            id: slPerformance
            from: 0
            to: 10
            value: 10
            stepSize: 1
            snapMode: Slider.SnapAlways
            onValueChanged: {
                config.performance = 10 - value
                slLabel.text = 100 - config.performance * 5 + " %"
            }
        }
        ColorLabel {
            id: slLabel
            text: "100 %"
        }
        InfoTip { tiptext: qsTr("Constrains the computing performance by picking a random result out of the best moves") }
    }
}
