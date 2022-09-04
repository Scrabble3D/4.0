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

    function timeStringToSeconds(aString) {
        const sParts = aString.split(":");
        var nResult = 0;
        var nTime;

        if (sParts.length === 3) {
            nTime = parseInt(sParts[0]);
            if (nTime < 24)
                nResult += nTime * 3600;
            nTime = parseInt(sParts[1]);
            if (nTime < 60)
                nResult += nTime * 60;
            nTime = parseInt(sParts[2]);
            if (nTime < 60)
                nResult += nTime;
        }
        return nResult;
    }

    Label {
        id: lbTimeControl
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        Layout.topMargin: rbNoLimit.topPadding
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
            onCheckedChanged: if (checked) config.timeControl = 0
        }
        RadioButton {
            id: rbPerMove
            text: qsTr("Per Move")
            onCheckedChanged: if (checked) {
                config.timeControl = 1
                tiPerMove.textChanged()
            }
        }
        TextField {
            id: tiPerMove
            enabled: rbPerMove.checked
            property bool accepted
            color: enabled ? myPalette.windowText : myPalette.midlight
            text: "0:01:00" //1 min
            background: Rectangle {
                id: bgPerMove
                width: 100
                color: tiPerMove.enabled
                       ? (tiPerMove.accepted)
                          ? "red" : config.myPalette.base
                       : config.myPalette.mid
            }
            onTextChanged: if (rbPerMove.checked) {
                config.timeControlValue = timeStringToSeconds(tiPerMove.text)
                tiPerMove.accepted = config.timeControlValue === 0
            }
            validator: RegularExpressionValidator { regularExpression: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d)$/ }
        }
        RadioButton {
            id: rbPerGame
            text: qsTr("Per Game")
            onCheckedChanged: if (checked) {
                config.timeControl = 2
                tiPerGame.textChanged()
            }
        }
        TextField {
            id: tiPerGame
            enabled: rbPerGame.checked
            property bool accepted
            color: enabled ? myPalette.windowText : myPalette.midlight
            text: "1:00:00" //1 h
            background: Rectangle {
                id: bgPerGame
                width: 100
                color: tiPerGame.enabled
                       ? (tiPerGame.accepted)
                          ? "red" : config.myPalette.base
                       : config.myPalette.mid
            }
            onTextChanged: if (rbPerGame.checked) {
                config.timeControlValue = timeStringToSeconds(tiPerGame.text)
                tiPerGame.accepted = config.timeControlValue === 0
            }
            validator: RegularExpressionValidator { regularExpression: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d)$/ }
        }
    }
    Label {
        id: lbPenalty
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        Layout.topMargin: rbGameEnd.topPadding
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
            Layout.leftMargin: 24
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
                color: enabled ? config.myPalette.windowText : config.myPalette.mid
                text: qsTr("points")
            }
            Label {
                id: lbPenltyJoiner
                Layout.columnSpan: 2
                enabled: rbPenalty.checked
                color: enabled ? config.myPalette.windowText : config.myPalette.mid
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
                color: enabled ? config.myPalette.windowText : config.myPalette.mid
                text: qsTr("times")
            }
        }
        CheckBox {
            id: cbGameEnd
            text: qsTr("Game lost after last timeout")
        }
    }
}
