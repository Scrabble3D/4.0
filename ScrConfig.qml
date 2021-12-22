import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Item {

    property alias markers: cbMarkers.checked
    property alias colorplayers: cbPlayerColors.checked
    property ListModel colors: ({})
    property ListModel playercolors: ({})
    property alias fieldCount: sbSize.value
    property var fields: []
    property alias is3D: rb3D.checked
    property alias numberOfLettersOnRack: sbPieces.value
    property alias letterSet: tvLetterSet.model
    property alias numberOfJokers: sbJoker.value

    ListModel {
        id: scrColors
        dynamicRoles: true
    }
    ListModel {
        id: scrPlayerColors
        dynamicRoles: true
    }
    Component.onCompleted: {
        //TODO: move colors into defaults; allow other bonus fields/colors
        scrColors.append({"itemColor":Qt.rgba(192/255,192/255,192/255,1),"itemName":qsTr("Start")}) //"#C0C0C0"
        scrColors.append({"itemColor":Qt.rgba(0,128/255,0,1),"itemName":qsTr("Default")}) //"#008000"
        scrColors.append({"itemColor":Qt.rgba(0,255/255,255/255,1),"itemName":qsTr("2x letter")}) //"#00FFFF"
        scrColors.append({"itemColor":Qt.rgba(0,0,255/255,1),"itemName":qsTr("3x letter")}) //"#0000FF"
        scrColors.append({"itemColor":Qt.rgba(0,0,96/255,1),"itemName":qsTr("4x letter")}) //"#000060"
        scrColors.append({"itemColor":Qt.rgba(255/255,96/255,96/255,1),"itemName":qsTr("2x word")}) //"#FF8080"
        scrColors.append({"itemColor":Qt.rgba(255/255,0,0,1),"itemName":qsTr("3x word")}) //"#FF0000"
        scrColors.append({"itemColor":Qt.rgba(96/255,0,0,1),"itemName":qsTr("4x word")}) //"#600000"
        scrColors.append({"itemColor":Qt.rgba(255/255,255/255,0,1),"itemName":qsTr("Pieces")}) //"#FFFF00"
        colors = scrColors
        scrPlayerColors.append({"itemColor":Qt.rgba(1,0,0,1),"itemName":qsTr("First player")})
        scrPlayerColors.append({"itemColor":Qt.rgba(0,1,0,1),"itemName":qsTr("Second player")})
        scrPlayerColors.append({"itemColor":Qt.rgba(0,0,1,1),"itemName":qsTr("Third player")})
        scrPlayerColors.append({"itemColor":Qt.rgba(1,0.75,0,1),"itemName":qsTr("Fourth player")})
        playercolors = scrPlayerColors

        fields = defaults.fieldsClassicScrabble.slice()
        cbLetterSet.currentIndex = 1

        cbDefaultFields.currentIndex = 0 //todo: not 3d by default
    }
/*
    header: Label {
        text: qsTr("Configuration")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }
*/
    Layout.fillWidth: true
    Layout.fillHeight: true

    onFieldsChanged: {
        var comp = JSON.stringify(fields)
        if (comp == JSON.stringify(defaults.fieldsClassicScrabble))
            cbDefaultFields.currentIndex = 0
        else if (comp == JSON.stringify(defaults.fieldsSuperScrabble))
            cbDefaultFields.currentIndex = 1
        else if (comp == JSON.stringify(defaults.fieldsClassicScrabble3D))
            cbDefaultFields.currentIndex = 2
        else if (comp == JSON.stringify(defaults.fieldsSuperScrabble3D))
            cbDefaultFields.currentIndex = 3
        else
            cbDefaultFields.currentIndex = 4;
    }

    SplitView {
        id: splitView
        anchors.fill: parent

        layer.enabled: true //TODO: shadow on the panels breaks splitview
        layer.effect: DropShadow {
            transparentBorder: true
            color: "lightgrey"
            horizontalOffset: 2
            verticalOffset: 2
            radius: 5
        }

        Rectangle {
            id: leftPane
            SplitView.preferredWidth: parent.width * 1/3
            SplitView.minimumWidth: 50
            ListView {
                id: lView
                anchors.fill: parent
                anchors.margins: 10
                Component {
                    id: lViewDelegates
                    Rectangle {
                        width: parent.width
                        height: 32
                        color: ListView.isCurrentItem ? palette.highlight : "transparent"
                        Text {
                            id: delegateText
                            text: name
                            leftPadding: 8
                            anchors.verticalCenter: parent.verticalCenter
                            color: lView.currentIndex == index ? palette.highlightedText : palette.base //TODO: WindowText = white, Base = black?
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: lView.currentIndex = index
                        }
                    }
                }

                model: ListModel {
                    ListElement { name: qsTr("Board") }
                    ListElement { name: qsTr("Letters") }
                    ListElement { name: qsTr("Rules") }
                    ListElement { name: qsTr("Dictionary") }
                }
                delegate: lViewDelegates
            }
        }

        Rectangle {
            id: rightPane
            SplitView.minimumWidth: 100
            SplitView.fillWidth: true
            ScrollView {
                id: scrollView
                anchors.fill: parent
//                onWidthChanged: console.log("pane: ", width)
//                width: rightPane.width
//                height: rightPane.height
                contentWidth: boardLayout.width //TODO: should depend on the actual view
                contentHeight: boardLayout.height
                clip: true
                GridLayout {
                    id: boardLayout
                    visible: lView.currentIndex == 0
                    columns: 2
                    columnSpacing: 8
                    rowSpacing: 8
                    Label {
                        id: lbPreset
                        leftPadding: 8
                        text: qsTr("Preset:")
                    }
                    ComboBox {
                        id: cbDefaultFields
                        Layout.minimumWidth: 200 //TODO: implicitContentWidthPolicy: ComboBox.WidestText not working
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
                                  fields = defaults.fieldsClassicScrabble.slice()
                                  rb2D.checked = true
                                  break
                              case 1:
                                  fields = defaults.fieldsSuperScrabble.slice()
                                  rb2D.checked = true
                                  break
                              case 2:
                                  fields = defaults.fieldsClassicScrabble3D.slice()
                                  rb3D.checked = true
                                  break
                              case 3:
                                  fields = defaults.fieldsSuperScrabble3D.slice()
                                  rb3D.checked = true
                                  break
                              case 4:
                                  break
                            }
                            if (currentIndex < 2)
                                sbSize.value = Math.sqrt(fields.length)
                            else if (currentIndex < 4)
                                sbSize.value = Math.round(Math.cbrt(fields.length)) //cubic root = 14.99999
                        }
                    }
                    Label {
                        id: lbDimensions
                        leftPadding: 8
                        text: qsTr("Dimension:")
                    }
                    RowLayout {
                        id: rl2D3D
                        RadioButton {
                            id: rb2D
                            checked: true
                            text: qsTr("2D")
                        }
                        RadioButton {
                            id: rb3D
                            text: qsTr("3D")
                        }
                    }
                    Label {
                        id: lbBoardSize
                        leftPadding: 8
                        text: qsTr("Board size:")
                    }
                    SpinBox {
                        id: sbSize
                        onValueChanged: {
                            var tmp = [] //new Array(sbSize.value*sbSize.value)
                            var lastSize
                            var i,j,k,z,v
                            if (rb3D.checked)
                            {
                                lastSize = Math.round(Math.cbrt(fields.length))
                                for (i=0; i<sbSize.value; i++)
                                 for (j=0; j<sbSize.value; j++)
                                  for (k=0; k<sbSize.value; k++) {
                                     z = i*lastSize*lastSize+j*lastSize+k  //TODO: doesnt work
                                     v = i*sbSize.value*sbSize.value+j*sbSize.value+k
                                     if (isNaN(fields[z]))
                                         tmp[v] = 1
                                     else
                                         tmp[v] = fields[z]
                                 }

                            } else
                            {
                                lastSize = Math.sqrt(fields.length)
                                for (i=0; i<sbSize.value; i++)
                                 for (j=0; j<sbSize.value; j++) {
                                     z = i*lastSize+j
                                     v = i*sbSize.value+j
                                     if (isNaN(fields[z]))
                                         tmp[v] = 1
                                     else
                                         tmp[v] = fields[z]
                                 }
                            }
                            fields = tmp.slice()
                        }
                    }
                    Label {
                        id: lbBonusFields
                        leftPadding: 8
                        text: qsTr("Bonus fields:")
                        verticalAlignment: Qt.AlignTop
                    }
                    GridLayout {
                        id: glBonusFields
                        columns: sbSize.value
                        columnSpacing: 1
                        rowSpacing: 1
                        Repeater {
                            model: sbSize.value * sbSize.value
                            Rectangle {
                                Layout.minimumWidth: 20
                                Layout.minimumHeight: 20
//                                Layout.preferredWidth: rightPane.width / sbSize.value //TODO: size to fit
//                                Layout.preferredHeight: Layout.preferredWidth
                                property int i: index+sbSize.value*sbSize.value*(pos3D.value-1)
                                color: colors.get(fields[i]).itemColor
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        var tmp = []
                                        tmp = fields
                                        tmp[i] = (fields[i]+1) % 8
                                        fields = tmp //to call refresh handler
                                    }
                                }
                            }
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
                            to: rb3D.checked ? Math.round(Math.cbrt(fields.length)) : 1
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
                    }
                    CheckBox {
                        id: cbMarkers
                        text: qsTr("Enabled")
                        checkState: Qt.Checked
                    }
                    Label {
                        id: lbColors
                        leftPadding: 8
                        text: qsTr("Colors:")
                    }
                    ColorPicker {
                        id: cpColors
                        pickerColors: colors
                    }
                    Label {
                        id: lbPlayerColors
                        leftPadding: 8
                        text: qsTr("Player colors:")
                    }
                    ColorPicker {
                        id: cpPlayerColors
                        pickerColors: playercolors
                    }
                    Label {
                        id: lbDummy
                        text: ""
                    }
                    CheckBox {
                        id: cbPlayerColors
                        text: qsTr("Use on board")
                        checkState: Qt.Checked
                    }

                }
                GridLayout {
                    id: configLetter
                    visible: lView.currentIndex === 1
                    columns: 2
                    columnSpacing: 8
                    rowSpacing: 8
                    RowLayout {
                        id: layoutCenteredPreset
                        Layout.columnSpan: 2
                        Layout.fillWidth: parent
                        Layout.alignment: Qt.AlignRight
                        Layout.bottomMargin: 12
                        Label {
                            id: lbLetterSet
                            text: qsTr("Letter set:")
                        }
                        ComboBox {
                            id: cbLetterSet
                            Layout.minimumWidth: 200 //TODO: implicitContentWidthPolicy: ComboBox.WidestText not working
                            Layout.preferredWidth: 300
                            model: defaults.languages
                            textRole: "nativeName"
                            onCurrentIndexChanged: {
                                tvLetterSet.model = defaults.languages[currentIndex].letters
                                sbJoker.value = defaults.languages[currentIndex].numberOfJokers
                                sbPieces.value = defaults.languages[currentIndex].numberOfLetters
                                sbRandoms.value = defaults.languages[currentIndex].numberOfRandoms
                                rbReadingDirectionLTR.checked = defaults.languages[currentIndex].readingDirection === Qt.LeftToRight
                                rbReadingDirectionRTL.checked = defaults.languages[currentIndex].readingDirection === Qt.RightToLeft
                            }
                        }
                    }
                    Label {
                        id: lbLetterDistribution
                        Layout.alignment: Qt.AlignTop | Qt.AlignRight
                        Layout.topMargin: horizontalHeader.height/2 - font.pixelSize /2
                        text: qsTr("Letter distribution:")
                    }
                    ColumnLayout {
                        id: layoutLetterSet
                        Layout.leftMargin: 8
                        Layout.fillHeight: true //TODO does not work?
                        HorizontalHeaderView {
                            id: horizontalHeader
                            syncView: tvLetterSet
                            model: [qsTr("Letter"), qsTr("Points"), qsTr("Count")]
                        }
                        TableView {
                            id: tvLetterSet //TODO: should be editable
                            Layout.preferredWidth: contentWidth + sbLetterSet.width
                            Layout.minimumHeight: 200
                            Layout.fillHeight: true
                            ScrollBar.vertical: ScrollBar {
                                id: sbLetterSet
                            }
                            clip: true
                            model: {} //set via cbLetterSet
                            delegate: Rectangle {
                                implicitWidth: 75
                                implicitHeight: 20
                                color: "transparent"
                                border.color: "lightgrey"
                                Text {
                                    text: display
                                    anchors.centerIn: parent
                                }
                            }
                            onModelChanged: {
                                var z = 0
                                if (model)
                                    for (var i=0; i<model.rowCount; i++)
                                        z += model.getRow(i).count
                                lbNumberOfLetters.text = qsTr("Total number of letters: %1").arg(z)
                            }
                        }
                        Label {
                            id: lbNumberOfLetters
                        }
                    }
                    Label {
                        id: lbJoker
                        Layout.leftMargin: 8
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Number of blank tiles:")
                    }
                    SpinBox {
                        id: sbJoker
                    }
                    Label {
                        id: lbPieces
                        Layout.leftMargin: 8
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Number of letters on rack:")
                    }
                    SpinBox {
                        id: sbPieces
                    }
                    Label {
                        id: lbRandoms
                        Layout.leftMargin: 8
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Number of random letters:")
                    }
                    SpinBox {
                        id: sbRandoms
                    }
                    Label {
                        id: lbReadingDirection
                        Layout.leftMargin: 8
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Reading direction:")
                    }
                    RowLayout {
                        id: layoutReadingDirection
                        Label {
                            text: qsTr("left to right")
                        }
                        RadioButton {
                            id: rbReadingDirectionLTR
                        }
                        Label {
                            text: qsTr("right to left")
                        }
                        RadioButton {
                            id: rbReadingDirectionRTL
                        }
                    }
                }
                GridLayout {
                    id: configRules
                    visible: lView.currentIndex === 2
                    columns: 2
                    columnSpacing: 8
                    rowSpacing: 8
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
                            checked: defaults.languages[cbLetterSet.currentIndex].timeControl == 0
                        }
                        RadioButton {
                            id: rbPerMove
                            text: qsTr("Per Move")
                            checked: defaults.languages[cbLetterSet.currentIndex].timeControl == 1
                        }
                        TextInput {
                            id: tiPerMove
                            enabled: rbPerMove.checked
                            color: enabled ? "black" : "grey"
                            text: "1:00:00"
                            validator: RegularExpressionValidator { regularExpression: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d)$/ }
                        }
                        RadioButton {
                            id: rbPerGame
                            text: qsTr("Per Game")
                            checked: defaults.languages[cbLetterSet.currentIndex].timeControl == 2
                        }
                        TextInput {
                            id: tiPerGame
                            enabled: rbPerGame.checked
                            color: enabled ? "black" : "grey"
                            text: defaults.languages[cbLetterSet.currentIndex].timePerGame
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
                            checked: defaults.languages[cbLetterSet.currentIndex].timeControlEnd
                        }
                        RadioButton {
                            id: rbPanelty
                            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                            text: qsTr("Penalty on timeout")
                            checked: defaults.languages[cbLetterSet.currentIndex].timeControlBuy
                        }
                        GridLayout {
                            id: layoutPenaltyPoints
                            columns: 2
                            SpinBox {
                                id: sbPenaltyPoints
                                enabled: rbPanelty.checked
                                value: defaults.languages[cbLetterSet.currentIndex].penaltyValue
                                from: 1
                                to: 5000
                            }
                            Label {
                                id: lbPenaltyPointsUnit
                                enabled: rbPanelty.checked
                                color: enabled ? "black" : "grey"
                                text: qsTr("points")
                            }
                            Label {
                                id: lbPenltyJoiner
                                Layout.columnSpan: 2
                                enabled: rbPanelty.checked
                                color: enabled ? "black" : "grey"
                                text: qsTr("but not more than")
                            }
                            SpinBox {
                                id: sbPenaltyCount
                                enabled: rbPanelty.checked
                                value: defaults.languages[cbLetterSet.currentIndex].penaltyCount
                                from: 1
                                to: 5000
                            }
                            Label {
                                id: lbPenaltyCountUnit
                                enabled: rbPanelty.checked
                                color: enabled ? "black" : "grey"
                                text: qsTr("times")
                            }
                        }
                        CheckBox {
                            id: cbGameEnd
                            text: qsTr("Game lost after last timeout")
                            checked: defaults.languages[cbLetterSet.currentIndex].gameLostByTime
                        }
                    }
                 }
                GridLayout {
                    id: configDictionary
                    columns: 2
                    width: scrollView.width
                    height: scrollView.height
                    onWidthChanged: dictTable.forceLayout()
                    visible: lView.currentIndex === 3

                    property int selectedDic: -1

//                    rows: 3
//                    columnSpacing: 8
//                    rowSpacing: 8
                    HorizontalHeaderView {
                        id: dictHeader
                        syncView: dictTable
                        model: GamePlay.dictModel
                        Layout.leftMargin: 8
                        Layout.columnSpan: 2
                    }
                    TableView {
                        id: dictTable
                        Layout.fillHeight: true
                        Layout.preferredWidth: contentWidth
                        Layout.leftMargin: 8
                        Layout.columnSpan: 2
                        ScrollBar.vertical: ScrollBar {}
                        clip: true;
                        columnWidthProvider: function(column) {
                            return column < 2
                                    ? configDictionary.width * 1/4
                                    : configDictionary.width * 1/8
                        }
                        model: GamePlay.dictModel
                        selectionModel: ItemSelectionModel {
                            id: dictSelect
                            model: dictTable.model
                            onSelectionChanged: console.log("changed")
                        }
                        delegate: Rectangle {
                            id: dictDelegate
                            required property bool selected
                            implicitWidth: parent.width
                            implicitHeight: text.height + 2
                            color: isSelected ? palette.highlight
                                              : selected ? Qt.lighter(palette.highlight) : "transparent"
                            border.color: "lightgrey"

                            function isDark(aColor) {
                                var temp = Qt.darker(aColor, 1) //Force conversion to color QML type object
                                var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
                                return temp.a > 0 && a >= 0.3
                            }

                            Text {
                                id: text
                                width: parent.width
                                padding: 2
                                color: isDark(parent.color) ? "white" : "black"
                                horizontalAlignment: Text.AlignHCenter
                                elide: Text.ElideRight
                                text: display
                            }
                            MouseArea {
                                anchors.fill: parent
                                onDoubleClicked: {
                                    configDictionary.selectedDic = -1 //don't load again via action button
                                    var fileName = GamePlay.dictModel.data(GamePlay.dictModel.index(row,2))
                                    var dicName = GamePlay.dictModel.data(GamePlay.dictModel.index(row,0))
                                    if (GamePlay.loadDictionary(fileName))
                                        GamePlay.addMessage(qsTr("Dictionary %1 successfully loaded.").arg(dicName))
                                }
                                onClicked: {
                                    isSelected ? configDictionary.selectedDic = -1
                                               : configDictionary.selectedDic = model.row
                                    authorLabel.text = author
                                    licenseLabel.text = license
                                    commentLabel.text = comment
                                    releaseLabel.text = release
                                    dictSelect.select(GamePlay.dictModel.index(model.row,0),ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows)
                                }
                            }
                        }
                    }

                    RoundButton {
                        id: actionButton
                        implicitWidth: 50
                        implicitHeight: 50
                        Layout.columnSpan: 2
                        display: AbstractButton.IconOnly
                        icon.source: "qrc:///resources/dictionary.png"
                        background: Rectangle {
                            anchors.fill: parent
                            radius: width / 2
                            gradient: Gradient {
                                GradientStop {
                                    position: actionButton.hovered ? 1.0 : 1.0
                                    color: actionButton.enabled ? "darkgreen" : "lightgrey"
                                }
                                GradientStop {
                                    position: actionButton.hovered ? 1.0 : 0.0
                                    color: actionButton.enabled ? "limegreen" : "lightgrey"
                                }
                            }
                        }
                        ToolTip {
                            text: qsTr("Load dictionary")
                            visible: actionButton.hovered
                            delay: 1000
                            timeout: 5000
                        }
                        anchors.right: dictTable.right
                        anchors.bottom: dictTable.bottom
                        anchors.rightMargin: -25
                        anchors.bottomMargin: -25
                        enabled: configDictionary.selectedDic > -1
                        //TODO: make an action
                        onPressed: {
                            var fileName = GamePlay.dictModel.data(GamePlay.dictModel.index(configDictionary.selectedDic,2))
                            var dicName = GamePlay.dictModel.data(GamePlay.dictModel.index(configDictionary.selectedDic,0))
                            if (GamePlay.loadDictionary(fileName))
                                GamePlay.addMessage(qsTr("Dictionary %1 successfully loaded.").arg(dicName))
                            configDictionary.selectedDic = -1
                        }
                    }


                    Label {
                        Layout.leftMargin: 16
                        text: qsTr("Author:"); }
                    Label { id: authorLabel }

                    Label {
                        Layout.leftMargin: 16
                        text: qsTr("License:"); }
                    Label { id: licenseLabel }

                    Label {
                        Layout.leftMargin: 16
                        text: qsTr("Release:"); }
                    Label { id: releaseLabel }

                    Label {
                        Layout.leftMargin: 16
                        Layout.bottomMargin: 32
                        Layout.alignment: Qt.AlignTop
                        text: qsTr("Comment:"); }
                    Label {
                        Layout.bottomMargin: 32
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        id: commentLabel }

                }

            }
        }
    }

}
