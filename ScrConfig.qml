import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Page {

    property alias markers: cbMarkers.checked
    property ListModel colors: ({})
    property alias fieldCount: sbSize.value
    property var fields: []
//    property alias fieldType: FieldType
    property alias is3D: rb3D.checked
    property alias numberOfLettersOnRack: sbPieces.value
    property alias letterSet: tvLetterSet.model
    property alias numberOfJokers: sbJoker.value

//    enum FieldType {Normal=0,Green=1,Piece=8}

    ListModel {
        id: scrColors
        dynamicRoles: true
    }
    Component.onCompleted: {
        //TODO: move colors into defaults
        scrColors.append({"itemColor":Qt.rgba(192/255,192/255,192/255,1),"itemName":qsTr("Start")}) //"#C0C0C0"
        scrColors.append({"itemColor":Qt.rgba(0,128/255,0,1),"itemName":qsTr("Default")}) //"#008000"
        scrColors.append({"itemColor":Qt.rgba(0,255/255,255/255,1),"itemName":qsTr("1x letter")}) //"#00FFFF"
        scrColors.append({"itemColor":Qt.rgba(0,0,255/255,1),"itemName":qsTr("2x letter")}) //"#0000FF"
        scrColors.append({"itemColor":Qt.rgba(0,0,96/255,1),"itemName":qsTr("3x letter")}) //"#000060"
        scrColors.append({"itemColor":Qt.rgba(255/255,96/255,96/255,1),"itemName":qsTr("1x word")}) //"#FF8080"
        scrColors.append({"itemColor":Qt.rgba(255/255,0,0,1),"itemName":qsTr("2x word")}) //"#FF0000"
        scrColors.append({"itemColor":Qt.rgba(96/255,0,0,1),"itemName":qsTr("3x word")}) //"#600000"
        scrColors.append({"itemColor":Qt.rgba(255/255,255/255,0,1),"itemName":qsTr("Pieces")}) //"#FFFF00"
        colors = scrColors
        fields = defaults.fieldsClassicScrabble.slice()
        cbLetterSet.currentIndex = 1
    }

    header: Label {
        text: qsTr("Configuration")
        font.pixelSize: Qt.application.font.pixelSize * 2
        padding: 10
    }

    RowLayout {
        anchors.fill: parent
        Rectangle {
            id: leftPane
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 10
            Layout.topMargin: 10
            Layout.bottomMargin: 10
            Layout.minimumWidth: 50
            ListView {
                id: lview
                anchors.fill: parent
                anchors.margins: 10
                implicitWidth: contentItem.childrenRect.width
                model: ListModel {
                    ListElement { name: qsTr("Board") }
                    ListElement { name: qsTr("Letters") }
                    ListElement { name: qsTr("Rules") }
                    ListElement { name: qsTr("Dictionary") }
                }
                delegate: ItemDelegate {
                    width: parent.width
                    Text {
                        text: name
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: lview.currentIndex = index
                    }
                }
                highlight: Rectangle {
                    color: palette.highlight
                    radius: 1
                }
            }

            layer.enabled: true
            layer.effect: DropShadow {
                transparentBorder: true
                color: "lightgrey"
                horizontalOffset: 2
                verticalOffset: 2
                radius: 5
            }
        }

        Rectangle {
            id: rightPane
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 100
            Layout.rightMargin: 10
            Layout.topMargin: 10
            Layout.bottomMargin: 10
            Item {
                id: configBoard
                visible: lview.currentIndex === 0
                GridLayout {
                    id: boardLayout
                    columns: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Label {
                        id: lbPreset
                        leftPadding: 8
                        text: qsTr("Preset:")
                    }
                    ComboBox {
                        id: cbDefaultFields
                        implicitContentWidthPolicy: ComboBox.WidestText
                        model: [qsTr("Classic"), qsTr("Supperscrabble"), qsTr("Scrabble 3D"),qsTr("Superscrabble 3D"),qsTr("User-defined")]
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
                            //TODO: keep bonus distribution when adding new fields, meaning
                            //fields needs to be two/three-dimensional- at least while resizing
/*                            if (fields.length<sbSize.value*sbSize.value) {
                                for (var i=1; i<sbSize.value; i++)
                                    fields.splice(i*sbSize.value-1,0,7)
                                for (i=0; i<sbSize.value; i++)
                                    fields.splice(i+sbSize.value*sbSize.value,0,1)
                            } else {
                                for (i=1; i<sbSize.value; i++)
                                    fields.splice(i*sbSize.value,1)
                                for (i=0; i<sbSize.value; i++)
                                    fields.splice(i+(sbSize.value*sbSize.value),1)
                            }
*/
/*
                            var tmp = []
                            for (var i=0; i<sbSize.value; i++)
                             for (var j=0; j<sbSize.value; j++) {
                                 z = i*sbSize.value+j
                                 if (isNaN(fields[z]))
                                     tmp[z] = 1
                                 else
                                     tmp[z] = fields[z]
                             }
                             fields = tmp
*/
/*
                            for (var i=0; i<sbSize.value*sbSize.value; i++)
                                if (isNaN(fields[i]))
                                    fields[i]=1;
*/
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
//                                Layout.preferredWidth: rightPane.width / sbSize.value
//                                Layout.preferredHeight: Layout.preferredWidth
                                property int i: index+sbSize.value*sbSize.value*(pos3D.value-1)
                                color: colors.get(fields[i]).itemColor
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        var tmp = []
                                        tmp = fields
                                        tmp[i] = (fields[i]+1) % 7
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
                        text: qsTr("Bonus markers in 2D:")
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
                        pickerColors: colors
                    }
                } //ColumnLayout
            }
            Item {
                id: configLetter
                visible: lview.currentIndex === 1
                GridLayout {
                    Layout.fillHeight: true
                    columns: 2
                    ComboBox {
                        id: cbLetterSet
                        Layout.leftMargin: 8
                        Layout.columnSpan: 2
                        Layout.preferredWidth: width
                        implicitContentWidthPolicy: ComboBox.WidestTextWhenCompleted
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
                    Label {
                        Layout.alignment: Qt.AlignTop | Qt.AlignRight
                        Layout.topMargin: horizontalHeader.height/2 - font.pixelSize /2
                        text: qsTr("Letter distribution:")
                    }
                    ColumnLayout {
                        Layout.leftMargin: 8
                        Layout.fillHeight: true //does not work?
                        HorizontalHeaderView {
                            id: horizontalHeader
                            syncView: tvLetterSet
                            model: [qsTr("Letter"), qsTr("Points"), qsTr("Count")]
                        }
                        TableView {
                            id: tvLetterSet
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
                    id: rlRandoms
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
            }
            Item {
                id: configRules
                visible: lview.currentIndex === 2
                GridLayout {
                    columns: 2
                    Label {
                        Layout.alignment: Qt.AlignRight | Qt.AlignTop
                        Layout.leftMargin: 8
                        Layout.topMargin: 8
                        text: qsTr("Time control:")
                    }
                    GridLayout {
                        columns: 2
                        RadioButton {
                            id: rbNoLimit
                            Layout.columnSpan: 2
                            text: qsTr("No Limit")
                        }
                        RadioButton {
                            id: rbPerMove
                            text: qsTr("Per Move")
                        }
                        TextInput {
                            id: tiPerMove
                            enabled: rbPerMove.checked
                            color: enabled ? "black" : "grey"
                            text: "1:00:00"
                            validator: RegularExpressionValidator { regularExpression: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d)$/ }
                        }
/*                        Label {
                            enabled: rbPerMove.checked
                            color: enabled ? "black" : "grey"
                            text: qsTr("hh:mm:ss")
                        }
*/
                        RadioButton {
                            id: rbPerGame
                            text: qsTr("Per Game")
                        }
                        TextInput {
                            id: tiPerGame
                            enabled: rbPerGame.checked
                            color: enabled ? "black" : "grey"
                            text: "1:00:00"
                            validator: RegularExpressionValidator { regularExpression: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d)$/ }
                        }
                    }
                    Label {
                        Layout.alignment: Qt.AlignRight | Qt.AlignTop
                        Layout.leftMargin: 8
                        Layout.topMargin: 8
                        text: qsTr("Penalty:")
                    }
                    ColumnLayout {
                        RadioButton {
                            id: rbGameEnd
                            Layout.columnSpan: 2
                            text: qsTr("Game ends on timeout")
                        }
                        RadioButton {
                            id: rbPanelty
                            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                            text: qsTr("Penalty on timeout")
                        }
                        GridLayout {
                            columns: 2
                            SpinBox {
                                id: sbPenaltyPoints
                                enabled: rbPanelty.checked
                                value: 10
                                IntValidator{bottom: 1; top: 100;}
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
                                value: 2
                                IntValidator{bottom: 1; top: 50;}
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
                        }
                    }
                } //GridLayout
             }

            layer.enabled: true
            layer.effect: DropShadow {
                transparentBorder: true
                color: "lightgrey"
                horizontalOffset: 2
                verticalOffset: 2
                radius: 5
            }
        }
    } //RowLayout
/*
    onFieldsChanged: {

        function isEqual(array1, array2) {
            return array1.length === array2.length && array1.every(function(value, index) { return value === array2[index]})
        }

        if (isEqual(fields,defaults.fieldsClassicScrabble))
            cbDefaultFields.currentIndex = 0
        else if (isEqual(fields,defaults.fieldsSuperScrabble))
            cbDefaultFields.currentIndex = 1
        else if (isEqual(fields,defaults.fieldsClassicScrabble3D))
            cbDefaultFields.currentIndex = 2
        else if (isEqual(fields,defaults.fieldsSuperScrabble3D))
            cbDefaultFields.currentIndex = 3
        else
            cbDefaultFields.currentIndex = 4
    }
*/
    onWidthChanged: ;//console.log(rightPane.width)
} //Page
