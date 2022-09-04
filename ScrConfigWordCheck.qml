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

        RadioButton {
            id: rbTakeback
            Layout.columnSpan: 3
            text: qsTr("Takeback")
            checked: true
            onCheckedChanged: if (checked) config.wordCheckMode = 0
        }
        RadioButton {
            id: rbPoll
            Layout.columnSpan: 3
            text: qsTr("Poll")
            onCheckedChanged: if (checked) config.wordCheckMode = 1
        }
        RadioButton {
            id: rbChallenge
            Layout.columnSpan: 3
            text: qsTr("Challenge")
            onCheckedChanged: if (checked) config.wordCheckMode = 2
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
        Label {
            id: lbPeriodUnit
            enabled: rbChallenge.checked
            color: enabled ? config.myPalette.windowText : config.myPalette.mid
            text: qsTr("seconds")
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
        Label {
            id: lbPenaltyUnit
            enabled: rbChallenge.checked
            color: enabled ? config.myPalette.windowText : config.myPalette.mid
            text: qsTr("points")
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
        Label {
            id: lbsbBonusUnit
            enabled: rbChallenge.checked
            color: enabled ? config.myPalette.windowText : config.myPalette.mid
            text: qsTr("points")
        }
    }
    Label {
        id: lbOptions
        leftPadding: 8
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        Layout.topMargin: cbClabbers.topPadding
        text: qsTr("Options:")
    }
    CheckBox {
        id: cbClabbers
        text: qsTr("CLABBERS variant")
        onToggled: config.clabbers = checked
    }

}
