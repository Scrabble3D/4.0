import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {

    columns: 3
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

    component TimeField: TextField {
        id: control
        implicitWidth: 60 + 28

        property bool accepted

        validator: RegularExpressionValidator {
            regularExpression: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d)$/
        } //accept from 00:00:00 to 23:59:59

        color: enabled ? "black" : "darkgrey"

        background: Rectangle {
            color: control.enabled
                   ? (control.accepted)
                      ? "red" : "white"
                   : "lightgrey"
        }

    }

    ButtonGroup {
        id: rbTimeGroup
    }

    ColorLabel {
        id: lbTimeControl
        Layout.column: 0; Layout.row: 0
        Layout.alignment: Qt.AlignRight
        Layout.leftMargin: 8
        Layout.topMargin: 8
        text: qsTr("Time control:")
    }
    ColorRadioButton {
        id: rbNoLimit
        Layout.column: 1; Layout.row: 0
        Layout.topMargin: 8
        text: qsTr("No Limit")
        ButtonGroup.group: rbTimeGroup
        onCheckedChanged: if (checked) config.timeControl = 0
    }
    ColorRadioButton {
        id: rbPerMove
        Layout.column: 1; Layout.row: 1
        text: qsTr("Per Move")
        ButtonGroup.group: rbTimeGroup
        onCheckedChanged: if (checked) {
            config.timeControl = 1
            tiPerMove.textChanged()
        }
    }
    TimeField {
        id: tiPerMove
        Layout.column: 2; Layout.row: 1
        enabled: rbPerMove.checked
        text: "0:01:00" //1 min
        onTextChanged: if (rbPerMove.checked) {
            config.timeControlValue = timeStringToSeconds(tiPerMove.text)
            tiPerMove.accepted = config.timeControlValue === 0
        }
    }
    ColorRadioButton {
        id: rbPerGame
        Layout.column: 1; Layout.row: 2
        text: qsTr("Per Game")
        ButtonGroup.group: rbTimeGroup
        onCheckedChanged: if (checked) {
            config.timeControl = 2
            tiPerGame.textChanged()
        }
    }
    TimeField {
        id: tiPerGame
        Layout.column: 2; Layout.row: 2
        enabled: rbPerGame.checked
        text: "1:00:00" //1 h
        onTextChanged: if (rbPerGame.checked) {
            config.timeControlValue = timeStringToSeconds(tiPerGame.text)
            tiPerGame.accepted = config.timeControlValue === 0
        }
    }

    ButtonGroup { id: rbPenaltyGroup }

    ColorLabel {
        id: lbPenalty
        Layout.column: 0; Layout.row: 3
        Layout.alignment: Qt.AlignRight
        Layout.topMargin: 8
        text: qsTr("Penalty:")
    }
    RowLayout {
        Layout.column: 1; Layout.row: 3; Layout.columnSpan: 2
        Layout.topMargin: 8
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
        Layout.column: 1; Layout.row: 4; Layout.columnSpan: 2
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
    RowLayout {
        Layout.column: 1; Layout.row: 5; Layout.columnSpan: 2
        Layout.leftMargin: 24
        ColorSpinBox {
            id: sbPenaltyPoints
            enabled: rbPenalty.checked && rbPenalty.enabled
            from: 1
            to: 5000
            value: 1
            onValueChanged: config.buytimepenalty = value
        }
        ColorLabel {
            id: lbPenaltyPointsUnit
            enabled: rbPenalty.checked && rbPenalty.enabled
            text: qsTr("points")
        }
    }
    ColorLabel {
        id: lbPenltyJoiner
        Layout.column: 1; Layout.row: 6; Layout.columnSpan: 2
        Layout.leftMargin: 24
        enabled: rbPenalty.checked && rbPenalty.enabled
        text: qsTr("but not more than")
    }
    RowLayout {
        Layout.column: 1; Layout.row: 7; Layout.columnSpan: 2
        Layout.leftMargin: 24
        ColorSpinBox {
            id: sbPenaltyCount
            enabled: rbPenalty.checked && rbPenalty.enabled
            from: 1
            to: 5000
            value: 1
            onValueChanged: config.buytimecount = value
        }
        ColorLabel {
            id: lbPenaltyCountUnit
            enabled: rbPenalty.checked && rbPenalty.enabled
            text: qsTr("times")
        }
    }
    RowLayout {
        Layout.column: 1; Layout.row: 8; Layout.columnSpan: 2
        ColorCheckBox {
            id: cbGameEnd
            text: qsTr("Game lost after last timeout")
            enabled: rbPerGame.checked
            onCheckedChanged: config.timegamelost = checked
        }
        InfoTip { tiptext: qsTr("If checked the game result will be set to zero after the last timeout") }
    }

}
