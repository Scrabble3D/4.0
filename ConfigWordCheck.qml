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

    ColorLabel {
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
            ColorRadioButton {
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
            ColorRadioButton {
                id: rbPoll
                text: qsTr("Poll")
                ButtonGroup.group: radioGroup
                onCheckedChanged: if (checked) config.wordCheckMode = 1
            }
            InfoTip { tiptext: qsTr("All players in the group are asked to confirm if a placed word has not been found in a dictionary") }
        }
        RowLayout {
            Layout.columnSpan: 3
            ColorRadioButton {
                id: rbChallenge
                text: qsTr("Challenge")
                ButtonGroup.group: radioGroup
                onCheckedChanged: if (checked) config.wordCheckMode = 2
            }
            InfoTip { tiptext: qsTr("The move needs to be challenged manually by one player to start the verification, which runs as a poll then") }
        }
        ColorLabel {
            id: lbPeriod
            leftPadding: 24
            enabled: rbChallenge.checked
            text: qsTr("Period:")
        }
        ColorSpinBox {
            id: sbPeriod
            enabled: rbChallenge.checked
            from: 0 //0sec
            to: 300 //5min
            onValueChanged: config.wordCheckPeriod = value
        }
        RowLayout {
            ColorLabel {
                id: lbPeriodUnit
                enabled: rbChallenge.checked
                text: qsTr("seconds")
            }
            InfoTip { tiptext: qsTr("Time span in which the challenge can be started") }
        }
        ColorLabel {
            id: lbPenlty
            leftPadding: 24
            enabled: rbChallenge.checked
            text: qsTr("Penalty:")
        }
        ColorSpinBox {
            id: sbPenalty
            enabled: rbChallenge.checked
            from: 0 //points
            to: 100
            onValueChanged: config.wordCheckPenalty = value
        }
        RowLayout {
            ColorLabel {
                id: lbPenaltyUnit
                enabled: rbChallenge.checked
                text: qsTr("points")
            }
            InfoTip { tiptext: qsTr("Penalty given to the challenging player if the word is accepted") }
        }
        ColorLabel {
            id: lbBonus
            leftPadding: 24
            enabled: rbChallenge.checked
            text: qsTr("Bonus:")
        }
        ColorSpinBox {
            id: sbBonus
            enabled: rbChallenge.checked
            from: 0 //points
            to: 100
            onValueChanged: config.wordCheckBonus = value
        }
        RowLayout {
            ColorLabel {
                id: lbsbBonusUnit
                enabled: rbChallenge.checked
                text: qsTr("points")
            }
            InfoTip { tiptext: qsTr("Bonus given to the challenging player if the word is rejected") }
        }
    }
    ColorLabel {
        id: lbOptions
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        Layout.topMargin: cbClabbers.topPadding
        enabled: false
        text: qsTr("Options:")
    }
    RowLayout {
        ColorCheckBox {
            id: cbClabbers
            text: qsTr("CLABBERS variant")
            enabled: false
            onCheckStateChanged: config.clabbers = checked
        }
        InfoTip { tiptext: qsTr("Enable this option to allow scrambled letters like SCRABBLE -> CLABBERS\nThis option is not yet implemented") }
    }
}
