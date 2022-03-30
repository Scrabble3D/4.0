import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    columns: 2
    columnSpacing: 8
    rowSpacing: 8
    property int defaultLetterSet: 0
    onDefaultLetterSetChanged: {
        rbNoLimit.checked = defaults.languages[defaultLetterSet].timeControl === 0
        rbPerMove.checked = defaults.languages[defaultLetterSet].timeControl === 1
        rbPerGame.checked = defaults.languages[defaultLetterSet].timeControl === 2
        tiPerGame.text = defaults.languages[defaultLetterSet].timePerGame
        rbGameEnd.checked = defaults.languages[defaultLetterSet].timeControlEnd
        rbPenalty.checked = defaults.languages[defaultLetterSet].timeControlBuy
        sbPenaltyPoints.value = defaults.languages[defaultLetterSet].penaltyValue
        sbPenaltyCount.value = defaults.languages[defaultLetterSet].penaltyCount
        cbGameEnd.checked = defaults.languages[defaultLetterSet].gameLostByTime
    }

    property alias rbNoLimit: rbNoLimit
    property alias rbPerMove: rbPerMove
    property alias tiPerMove: tiPerMove
    property alias rbPerGame: rbPerGame
    property alias tiPerGame: tiPerGame
    property alias rbGameEnd: rbGameEnd
    property alias rbPenalty: rbPenalty
    property alias sbPenaltyPoints: sbPenaltyPoints
    property alias sbPenaltyCount: sbPenaltyCount
    property alias cbGameEnd: cbGameEnd

    Label {
        id: lbTimeControl
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: qsTr("Time control:")
    }
    GridLayout {
        id: layoutTimeControl
        columns: 2
        Layout.bottomMargin: 8
        RadioButton {
            id: rbNoLimit
            Layout.columnSpan: 2
            text: qsTr("No Limit")
        }
        RadioButton {
            id: rbPerMove
            text: qsTr("Per Move")
        }
        TextField {
            id: tiPerMove
            enabled: rbPerMove.checked
            color: enabled ? myPalette.windowText : myPalette.midlight
            text: "0:01:00" //1 min
            validator: RegularExpressionValidator { regularExpression: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d)$/ }
        }

        RadioButton {
            id: rbPerGame
            text: qsTr("Per Game")
        }
        TextField {
            id: tiPerGame
            enabled: rbPerGame.checked
            color: enabled ? myPalette.windowText : myPalette.midlight
            text: "1:00:00" //1 h
            validator: RegularExpressionValidator { regularExpression: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d)$/ }
        }
    }
    Label {
        id: lbPenalty
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        text: qsTr("Penalty:")
    }
    ColumnLayout {
        id: layoutPenalty
        RadioButton {
            id: rbGameEnd
            Layout.columnSpan: 2
            text: qsTr("Game ends on timeout")
        }
        RadioButton {
            id: rbPenalty
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            text: qsTr("Penalty on timeout")
        }
        GridLayout {
            id: layoutPenaltyPoints
            columns: 2
            SpinBox {
                id: sbPenaltyPoints
                enabled: rbPenalty.checked
                from: 1
                to: 5000
            }
            Label {
                id: lbPenaltyPointsUnit
                enabled: rbPenalty.checked
                color: enabled ? "black" : "grey"
                text: qsTr("points")
            }
            Label {
                id: lbPenltyJoiner
                Layout.columnSpan: 2
                enabled: rbPenalty.checked
                color: enabled ? "black" : "grey"
                text: qsTr("but not more than")
            }
            SpinBox {
                id: sbPenaltyCount
                enabled: rbPenalty.checked
                from: 1
                to: 5000
            }
            Label {
                id: lbPenaltyCountUnit
                enabled: rbPenalty.checked
                color: enabled ? "black" : "grey"
                text: qsTr("times")
            }
        }
        CheckBox {
            id: cbGameEnd
            text: qsTr("Game lost after last timeout")
        }
    }
 }
