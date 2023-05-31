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

    ColorLabel {
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
        ButtonGroup {id: rbTimeGroup}
        ColorRadioButton {
            id: rbNoLimit
            Layout.columnSpan: 2
            text: qsTr("No Limit")
            ButtonGroup.group: rbTimeGroup
            onCheckedChanged: if (checked) config.timeControl = 0
        }
        ColorRadioButton {
            id: rbPerMove
            text: qsTr("Per Move")
            ButtonGroup.group: rbTimeGroup
            onCheckedChanged: if (checked) {
                config.timeControl = 1
                tiPerMove.textChanged()
            }
        }
        ColorTextField {
            id: tiPerMove
            enabled: rbPerMove.checked
            text: "0:01:00" //1 min
            onTextChanged: if (rbPerMove.checked) {
                config.timeControlValue = timeStringToSeconds(tiPerMove.text)
                tiPerMove.accepted = config.timeControlValue === 0
            }
        }
        ColorRadioButton {
            id: rbPerGame
            text: qsTr("Per Game")
            ButtonGroup.group: rbTimeGroup
            onCheckedChanged: if (checked) {
                config.timeControl = 2
                tiPerGame.textChanged()
            }
        }
        ColorTextField {
            id: tiPerGame
            enabled: rbPerGame.checked
            text: "1:00:00" //1 h
            onTextChanged: if (rbPerGame.checked) {
                config.timeControlValue = timeStringToSeconds(tiPerGame.text)
                tiPerGame.accepted = config.timeControlValue === 0
            }
        }
    }
    ColorLabel {
        id: lbPenalty
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        Layout.topMargin: rbGameEnd.topPadding
        text: qsTr("Penalty:")
    }
    ColumnLayout {
        id: layoutPenalty
        ButtonGroup { id: rbPenaltyGroup }
        RowLayout {
            ColorRadioButton {
                id: rbGameEnd
                Layout.columnSpan: 2
                ButtonGroup.group: rbPenaltyGroup
                text: qsTr("Game ends on timeout")
                enabled: rbPerGame.checked
                onCheckedChanged: config.canbuytime = false
            }
            InfoTip { tiptext: qsTr("Players who run out of time will pass automatically") }
        }
        RowLayout {
            ColorRadioButton {
                id: rbPenalty
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                ButtonGroup.group: rbPenaltyGroup
                text: qsTr("Penalty on timeout")
                enabled: rbPerGame.checked
                onCheckedChanged: config.canbuytime = true
            }
            InfoTip { tiptext: qsTr("You 'buy' one minute of time") }
        }
        GridLayout {
            id: layoutPenaltyPoints
            Layout.leftMargin: 24
            columns: 2
            ColorSpinBox {
                id: sbPenaltyPoints
                enabled: rbPenalty.checked && rbPenalty.enabled
                from: 1
                to: 5000
                onValueChanged: config.buytimepenalty = value
            }
            ColorLabel {
                id: lbPenaltyPointsUnit
                enabled: rbPenalty.checked && rbPenalty.enabled
                text: qsTr("points")
            }
            ColorLabel {
                id: lbPenltyJoiner
                Layout.columnSpan: 2
                enabled: rbPenalty.checked && rbPenalty.enabled
                text: qsTr("but not more than")
            }
            ColorSpinBox {
                id: sbPenaltyCount
                enabled: rbPenalty.checked && rbPenalty.enabled
                from: 1
                to: 5000
                onValueChanged: config.buytimecount = value
            }
            ColorLabel {
                id: lbPenaltyCountUnit
                enabled: rbPenalty.checked && rbPenalty.enabled
                text: qsTr("times")
            }
        }
        RowLayout {
            ColorCheckBox {
                id: cbGameEnd
                text: qsTr("Game lost after last timeout")
                enabled: rbPerGame.checked
                onCheckedChanged: config.timegamelost = checked
            }
            InfoTip { tiptext: qsTr("If checked the game result will be set to zero after the last timeout") }
        }
    }
}
