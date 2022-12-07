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
    }

    property alias sbPasses: sbPasses
    property alias sbBingo: sbBingo
    property alias sbGameEnd: sbGameEnd
    property alias cbAddLetters: cbAddLetters
    property alias cbSubstractLetters: cbSubstractLetters
    property alias sbJokerPenalty: sbJokerPenalty
    property alias cbChangeIsPass: cbChangeIsPass

    Label {
        id: lbPasses
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTopmitspieler
        text: qsTr("Passes until game end:")
    }
    RowLayout {
        SpinBox {
        id: sbPasses
        from: 1
        to: 99
        onValueChanged: config.numberOfPasses = value
    }
        InfoTip { tiptext: qsTr("The number of moves each player can pass consecutively until the game ends") }
    }
    Label {
        id: lbBingo
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: qsTr("Bonus for Scrabble/Bingos:")
    }
    RowLayout {
        SpinBox {
            id: sbBingo
            from: 0
            to: 999
            onValueChanged: config.bingoBonus = value
        }
        InfoTip { tiptext: qsTr("The 'bingo' bonus when all tiles from the rack are placed at once") }
    }
    Label {
        id: lbGameEnd
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: qsTr("Bonus on game end:")
    }
    RowLayout {
        SpinBox {
            id: sbGameEnd
            from: 0
            to: 999
            onValueChanged: config.gameEndBonus = value
        }
        InfoTip { tiptext: qsTr("Extra bonus awarded to the player who ends the game") }
    }
    Label {
        id: lbJokerPenalty
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: qsTr("Penalty for left blanks:")
    }
    RowLayout {
        SpinBox {
            id: sbJokerPenalty
            from: 0
            to: 999
            onValueChanged: config.jokerPenalty = value
        }
        InfoTip { tiptext: qsTr("Extra penalty for blank tile on the rack when the game ends") }
    }
    RowLayout {
        Layout.columnSpan: 2
        Layout.leftMargin: 8
        CheckBox {
            id: cbAddLetters
            text: qsTr("Add the value of other players letters")
            onToggled: config.addLetters = checked
        }
        InfoTip { tiptext: qsTr("The sum of remaining tiles from the opponents is added to the result of the player who ended the game") }
    }
    RowLayout {
        Layout.columnSpan: 2
        Layout.leftMargin: 8
        CheckBox {
            id: cbSubstractLetters
            text: qsTr("Substract the value of left letters")
            onToggled: config.substractLetters = checked
        }
        InfoTip { tiptext: qsTr("The value of the remaining tiles is deducted from each player's result") }
    }
    RowLayout {
        Layout.columnSpan: 2
        Layout.leftMargin: 8
        CheckBox {
            id: cbChangeIsPass
            text: qsTr("Treat exchange as pass")
            onToggled: config.changeIsPass = checked
        }
        InfoTip { tiptext: qsTr("You can decide whether an exchange of letters is counted as pass") }
    }
}
