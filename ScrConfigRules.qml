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
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: qsTr("Max passes:")
    }
    SpinBox {
        id: sbPasses
        from: 1
        to: 99
        onValueChanged: config.numberOfPasses = value
    }
    Label {
        id: lbBingo
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: qsTr("Bingo bonus:")
    }
    SpinBox {
        id: sbBingo
        from: 0
        to: 999
        onValueChanged: config.bingoBonus = value
    }
    Label {
        id: lbGameEnd
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: qsTr("Game end bonus:")
    }
    SpinBox {
        id: sbGameEnd
        from: 0
        to: 999
        onValueChanged: config.gameEndBonus = value
    }
    Label {
        id: lbJokerPenalty
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: qsTr("Penalty for left blanks:")
    }
    SpinBox {
        id: sbJokerPenalty
        from: 0
        to: 999
        onValueChanged: config.jokerPenalty = value
    }
    CheckBox {
        id: cbAddLetters
        Layout.columnSpan: 2
        text: qsTr("Add the value of other players letters")
        onToggled: config.addLetters = checked
    }
    CheckBox {
        id: cbSubstractLetters
        Layout.columnSpan: 2
        text: qsTr("Substract the value of left letters")
        onToggled: config.substractLetters = checked
    }
    CheckBox {
        id: cbChangeIsPass
        Layout.columnSpan: 2
        text: qsTr("Treat exchange as pass")
        onToggled: config.changeIsPass = checked
    }
}
