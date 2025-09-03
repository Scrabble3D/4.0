import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    //aliases required for save/loadConfig in ScrConfig
    property alias cbMarkers: cbMarkers
    property alias rb2D: rb2D
    property alias rb3D: rb3D
    property alias sbSize: sbSize
    property alias cbPlayerColors: cbPlayerColors

    //aliases for newgame infobox
    property string boardName: cbDefaultBoard.currentText
    columns: 2
    columnSpacing: 8
    rowSpacing: 8
    function checkDefault() {
        var comp = JSON.stringify(config.board)
        if (comp === JSON.stringify(defaults.boardClassicScrabble))
            cbDefaultBoard.currentIndex = 0
        else if (comp === JSON.stringify(defaults.boardSuperScrabble))
            cbDefaultBoard.currentIndex = 1
        else if (comp === JSON.stringify(defaults.boardClassicScrabble3D))
            cbDefaultBoard.currentIndex = 2
        else if (comp === JSON.stringify(defaults.boardSuperScrabble3D))
            cbDefaultBoard.currentIndex = 3
        else
            cbDefaultBoard.currentIndex = 4
    }

    //needed on Android with dark mode
    ColorLabel {
        id: lbPreset
        Layout.leftMargin: 8
        Layout.topMargin: 8
        text: qsTr("Preset:")
        Layout.alignment: Qt.AlignRight
    }
    ColorComboBox {
        id: cbDefaultBoard

        Layout.minimumWidth: 100
        Layout.preferredWidth: 250
        Layout.topMargin: 8

        model: [qsTr("Classic"), qsTr("Supperscrabble"), qsTr(
                "Scrabble 3D"), qsTr("Superscrabble 3D"), qsTr("User-defined")]

        onCurrentIndexChanged: {
            switch (currentIndex) {
            case 0:
                config.board = defaults.boardClassicScrabble.slice()
                rb2D.checked = true
                break
            case 1:
                config.board = defaults.boardSuperScrabble.slice()
                rb2D.checked = true
                break
            case 2:
                config.board = defaults.boardClassicScrabble3D.slice()
                rb3D.checked = true
                break
            case 3:
                config.board = defaults.boardSuperScrabble3D.slice()
                rb3D.checked = true
                break
            case 4:
                break
            }
            if (currentIndex < 2)
                sbSize.value = Math.sqrt(config.board.length)
            else if (currentIndex < 4)
                sbSize.value = Math.round(
                            Math.cbrt(
                                config.board.length)) //cubic root = 14.99999
        }
    }
    ColorLabel {
        id: lbDimensions
        leftPadding: 8
        text: qsTr("Dimension:")
        Layout.alignment: Qt.AlignRight
    }
    RowLayout {
        id: rl2D3D
        ColorRadioButton {
            id: rb2D
            checked: true
            text: qsTr("2D")
            onCheckedChanged: config.bIs3D = false
        }
        ColorRadioButton {
            id: rb3D
            text: qsTr("3D")
            onCheckedChanged: config.bIs3D = true
        }
    }
    ColorLabel {
        id: lbBoardSize
        leftPadding: 8
        text: qsTr("Board size:")
        Layout.alignment: Qt.AlignRight
    }
    ColorSpinBox {
        id: sbSize
        value: 15
        onValueChanged: {
            var tmp = [] //new Array(sbSize.value*sbSize.value)
            var lastSize
            var i, j, k, z, v
            if (rb3D.checked) {
                lastSize = Math.round(Math.cbrt(config.board.length))
                for (i = 0; i < sbSize.value; i++)
                    for (j = 0; j < sbSize.value; j++)
                        for (k = 0; k < sbSize.value; k++) {
                            z = i * lastSize * lastSize + j * lastSize + k
                            v = i * sbSize.value * sbSize.value + j * sbSize.value + k
                            if (isNaN(config.board[z]))
                                tmp[v] = 1
                            else
                                tmp[v] = config.board[z]
                        }
            } else {
                lastSize = Math.sqrt(config.board.length)
                for (i = 0; i < sbSize.value; i++)
                    for (j = 0; j < sbSize.value; j++) {
                        z = i * lastSize + j
                        v = i * sbSize.value + j
                        if (isNaN(config.board[z]))
                            tmp[v] = 1
                        else
                            tmp[v] = config.board[z]
                    }
            }
            config.board = tmp.slice()
            checkDefault()
        }
    }
    ColorLabel {
        id: lbBonusFields
        leftPadding: 8
        text: qsTr("Bonus fields:")
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
    }
    RowLayout {
        GridLayout {
            id: glBonusFields
            columns: sbSize.value
            columnSpacing: 0
            rowSpacing: 0
            antialiasing: true
            Repeater {
                model: sbSize.value * sbSize.value
                Rectangle {
                    Layout.minimumWidth: 10
                    Layout.minimumHeight: 10
                    property int nSize: Math.max(200, rightPane.width - 200)
                    Layout.preferredWidth: nSize / sbSize.value
                    Layout.preferredHeight: nSize / sbSize.value
                    property int i: index + sbSize.value * sbSize.value * (pos3D.value - 1)
                    color: colors.get(config.board[i]).itemColor
                    border.width: 0.1
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var tmp = []
                            tmp = config.board
                            tmp[i] = (config.board[i] + 1) % 8
                            config.board = tmp
                            checkDefault()
                        }
                    }
                }
            }
            onWidthChanged: {
                scrollView.contentHeight = configBoard.height + 50
                lbBonusFields.height = height
            }
        }
        InfoTip {
            Layout.alignment: Qt.AlignTop
            tiptext: qsTr("Click the fields to iterate through types")
        }
    }
    RowLayout {
        id: rl3DSlider
        Layout.column: 1
        Layout.row: 4
        visible: rb3D.checked
        ColorLabel {
            text: qsTr("Level:")
        }
        Slider {
            id: pos3D
            from: 1
            to: rb3D.checked ? Math.round(Math.cbrt(config.board.length)) : 1
            snapMode: Slider.SnapAlways
            stepSize: 1
        }
        ColorLabel {
            text: pos3D.value
        }
        InfoTip {
            tiptext: qsTr("Use the slider to access the levels in the 3D cube")
        }
    }
    ColorLabel {
        id: lbBonusMarker
        leftPadding: 8
        text: qsTr("Bonus markers:")
        Layout.alignment: Qt.AlignRight
    }
    ColorCheckBox {
        id: cbMarkers
        text: qsTr("Enabled")
        onCheckedChanged: config.bMarkers = checked
    }
    ColorLabel {
        id: lbColors
        leftPadding: 8
        text: qsTr("Colors:")
        Layout.alignment: Qt.AlignRight
    }
    ColorPicker {
        id: cpColors
        pickerColors: config.colors
    }
    ColorLabel {
        id: lbPlayerColors
        leftPadding: 8
        text: qsTr("Player colors:")
        Layout.alignment: Qt.AlignRight
    }
    ColorPicker {
        id: cpPlayerColors
        pickerColors: config.playercolors
    }
    ColorLabel {
        id: lbDummy
        text: ""
    }
    ColorCheckBox {
        id: cbPlayerColors
        text: qsTr("Use on board")
        checkState: Qt.Checked
        onCheckedChanged: config.bColoredPlayers = checked
    }
}
