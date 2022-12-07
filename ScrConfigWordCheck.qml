import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    columns: 2
    columnSpacing: 8
    rowSpacing: 8
    property int defaultLetterSet: 0
    onDefaultLetterSetChanged: {
        rbTakeback.checked = defaults.languages[defaultLetterSet].wordCheckMode === 0
        rbPoll.checked = defaults.languages[defaultLetterSet].wordCheckMode === 1
        rbChallenge.checked = defaults.languages[defaultLetterSet].wordCheckMode === 2
        sbPeriod.value = defaults.languages[defaultLetterSet].challengeTime
        sbPenalty.value = defaults.languages[defaultLetterSet].challengePenalty
    }
    property alias rbTakeback: rbTakeback
    property alias rbPoll: rbPoll
    property alias rbChallenge: rbChallenge
    property alias sbPeriod: sbPeriod
    property alias sbPenalty: sbPenalty
    property alias sbBonus: sbBonus
    property alias cbClabbers: cbClabbers

    Label {
        id: lMode
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        Layout.topMargin: rbTakeback.topPadding
        text: qsTr("Mode:")
    }
    GridLayout {
        id: layoutWordCheckModes
        columns: 3
        Layout.bottomMargin: 8
        ButtonGroup { id: radioGroup }
        RowLayout {
            Layout.columnSpan: 3
            RadioButton {
                id: rbTakeback
                text: qsTr("Takeback")
                checked: true
                ButtonGroup.group: radioGroup
                onCheckedChanged: if (checked) config.wordCheckMode = 0
            }
            InfoTip { tiptext: qsTr("The program checks the move and asks in case of unknown words for confirmation before the next player takes over") }
        }
        RowLayout {
            Layout.columnSpan: 3
            RadioButton {
                id: rbPoll
                text: qsTr("Poll")
                ButtonGroup.group: radioGroup
                onCheckedChanged: if (checked) config.wordCheckMode = 1
            }
            InfoTip { tiptext: qsTr("All players in the group are asked to confirm if a placed word has not been found in a dictionary") }
        }
        RowLayout {
            Layout.columnSpan: 3
            RadioButton {
                id: rbChallenge
                text: qsTr("Challenge")
                ButtonGroup.group: radioGroup
                onCheckedChanged: if (checked) config.wordCheckMode = 2
            }
            InfoTip { tiptext: qsTr("The move needs to be challenged manually by one player to start the verification, which runs as a poll then") }
        }
        Label {
            id: lbPeriod
            leftPadding: 24
            enabled: rbChallenge.checked
            color: enabled ? config.myPalette.windowText : config.myPalette.mid
            text: qsTr("Period:")
        }
        SpinBox {
            id: sbPeriod
            enabled: rbChallenge.checked
            from: 0 //0sec
            to: 300 //5min
            onValueChanged: config.wordCheckPeriod = value
        }
        RowLayout {
            Label {
                id: lbPeriodUnit
                enabled: rbChallenge.checked
                color: enabled ? config.myPalette.windowText : config.myPalette.mid
                text: qsTr("seconds")
            }
            InfoTip { tiptext: qsTr("Time span in which the challenge can be started") }
        }
        Label {
            id: lbPenlty
            leftPadding: 24
            enabled: rbChallenge.checked
            color: enabled ? config.myPalette.windowText : config.myPalette.mid
            text: qsTr("Penalty:")
        }
        SpinBox {
            id: sbPenalty
            enabled: rbChallenge.checked
            from: 0 //points
            to: 100
            onValueChanged: config.wordCheckPenalty = value
        }
        RowLayout {
            Label {
                id: lbPenaltyUnit
                enabled: rbChallenge.checked
                color: enabled ? config.myPalette.windowText : config.myPalette.mid
                text: qsTr("points")
            }
            InfoTip { tiptext: qsTr("Penalty given to the challenging player if the word is accepted") }
        }
        Label {
            id: lbBonus
            leftPadding: 24
            enabled: rbChallenge.checked
            color: enabled ? config.myPalette.windowText : config.myPalette.mid
            text: qsTr("Bonus:")
        }
        SpinBox {
            id: sbBonus
            enabled: rbChallenge.checked
            from: 0 //points
            to: 100
            onValueChanged: config.wordCheckBonus = value
        }
        RowLayout {
            Label {
                id: lbsbBonusUnit
                enabled: rbChallenge.checked
                color: enabled ? config.myPalette.windowText : config.myPalette.mid
                text: qsTr("points")
            }
            InfoTip { tiptext: qsTr("Bonus given to the challenging player if the word is rejected") }
        }
    }
    Label {
        id: lbOptions
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        Layout.topMargin: cbClabbers.topPadding
        enabled: false //TODO: CLABBERS
        text: qsTr("Options:")
    }
    RowLayout {
        CheckBox {
            id: cbClabbers
            text: qsTr("CLABBERS variant")
            enabled: false //TODO: CLABBERS
            onToggled: config.clabbers = checked
        }
        InfoTip { tiptext: qsTr("Enable this option to allow scrambled letters like SCRABBLE -> CLABBERS\nThis option is not yet implemented") }
    }
}
