import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    //aliases required for save/loadConfig in ScrConfig
    property alias cbMarkers: cbMarkers;
    property alias rb2D: rb2D;
    property alias rb3D: rb3D;
    property alias sbSize: sbSize;
    property alias cbPlayerColors: cbPlayerColors;

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
            cbDefaultBoard.currentIndex = 4;
    }

    Label {
        id: lbPreset
        leftPadding: 8
        text: qsTr("Preset:")
        Layout.alignment: Qt.AlignRight
    }
    ComboBox {
        id: cbDefaultBoard
        Layout.preferredWidth: 250
        model:
            [ qsTr("Classic"),
            qsTr("Supperscrabble"),
            qsTr("Scrabble 3D"),
            qsTr("Superscrabble 3D"),
            qsTr("User-defined")
        ]
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
                sbSize.value = Math.round(Math.cbrt(config.board.length)) //cubic root = 14.99999
        }
    }
    Label {
        id: lbDimensions
        leftPadding: 8
        text: qsTr("Dimension:")
        Layout.alignment: Qt.AlignRight
    }
    RowLayout {
        id: rl2D3D
        RadioButton {
            id: rb2D
            checked: true
            text: qsTr("2D")
            onCheckedChanged: config.bIs3D = false
        }
        RadioButton {
            id: rb3D
            text: qsTr("3D")
            onCheckedChanged: config.bIs3D = true
        }
    }
    Label {
        id: lbBoardSize
        leftPadding: 8
        text: qsTr("Board size:")
        Layout.alignment: Qt.AlignRight
    }
    SpinBox {
        id: sbSize
        onValueChanged: {
            var tmp = [] //new Array(sbSize.value*sbSize.value)
            var lastSize
            var i,j,k,z,v
            if (rb3D.checked)
            {
                lastSize = Math.round(Math.cbrt(config.board.length))
                for (i=0; i<sbSize.value; i++)
                    for (j=0; j<sbSize.value; j++)
                        for (k=0; k<sbSize.value; k++) {
                            z = i*lastSize*lastSize+j*lastSize+k
                            v = i*sbSize.value*sbSize.value+j*sbSize.value+k
                            if (isNaN(config.board[z]))
                                tmp[v] = 1
                            else
                                tmp[v] = config.board[z]
                        }

            } else
            {
                lastSize = Math.sqrt(config.board.length)
                for (i=0; i<sbSize.value; i++)
                    for (j=0; j<sbSize.value; j++) {
                        z = i*lastSize+j
                        v = i*sbSize.value+j
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
    Label {
        id: lbBonusFields
        leftPadding: 8
        text: qsTr("Bonus fields:")
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
    }
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
                //TODO: configboard: fit size to parent
                Layout.preferredWidth: 15// (scrollView.contentWidth - 50) / sbSize.value
                Layout.preferredHeight: 15//Layout.preferredWidth
                property int i: index+sbSize.value*sbSize.value*(pos3D.value-1)
                color: colors.get(config.board[i]).itemColor
                border.width: 0.1
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var tmp = []
                        tmp = config.board
                        tmp[i] = (config.board[i]+1) % 8
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
    RowLayout {
        id: rl3DSlider
        Layout.column: 1
        Layout.row: 4
        visible: rb3D.checked
        Label {
            text: qsTr("Level:")
        }
        Slider {
            id: pos3D
            from: 1
            to: rb3D.checked ? Math.round(Math.cbrt(config.board.length)) : 1
            snapMode: Slider.SnapAlways
            stepSize: 1
        }
        Label {
            text: pos3D.value
        }
    }
    Label {
        id: lbBonusMarker
        leftPadding: 8
        text: qsTr("Bonus markers:")
        Layout.alignment: Qt.AlignRight
    }
    CheckBox {
        id: cbMarkers
        text: qsTr("Enabled")
        onCheckedChanged: config.bMarkers = checked
    }
    Label {
        id: lbColors
        leftPadding: 8
        text: qsTr("Colors:")
        Layout.alignment: Qt.AlignRight
    }
    ColorPicker {
        id: cpColors
        pickerColors: config.colors
    }
    Label {
        id: lbPlayerColors
        leftPadding: 8
        text: qsTr("Player colors:")
        Layout.alignment: Qt.AlignRight
    }
    ColorPicker {
        id: cpPlayerColors
        pickerColors: config.playercolors
    }
    Label {
        id: lbDummy
        text: ""
    }
    CheckBox {
        id: cbPlayerColors
        text: qsTr("Use on board")
        checkState: Qt.Checked
        onCheckedChanged: config.bColoredPlayers = checked
    }
}
