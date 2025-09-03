import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

GridLayout {

    property alias cbLetterSet: cbLetterSet
    property alias sbJoker: sbJoker
    property alias sbPieces: sbPieces
    property alias sbRandoms: sbRandoms
    property alias rbReadingDirectionLTR: rbReadingDirectionLTR
    property alias rbReadingDirectionRTL: rbReadingDirectionRTL

    function getLetterSetName() {
        //used at ScrNewGame
        var sResult = cbLetterSet.currentText
        if (sResult === "")
            sResult = qsTr("Special")
        return sResult
    }

    function checkDefault() {
        var comp = JSON.stringify(getLetterSet(-1))
        // sets can be identical like en_UK / en_US
        if ((cbLetterSet.currentIndex > -1)
                && (comp === JSON.stringify(getLetterSet(
                                                cbLetterSet.currentIndex))))
            return

        for (var i = 0; i < defaults.languages.length; i++)
            if (comp === JSON.stringify(getLetterSet(i))) {
                cbLetterSet.currentIndex = i
                return
            }

        cbLetterSet.currentIndex = -1
    }

    function getLetterSet(index) {
        var letterlist = []
        if (index === -1)
            for (var i = 0; i < tmLetterSet.rowCount; i++) {
                letterlist.push(tmLetterSet.rows[i].letter)
                letterlist.push(tmLetterSet.rows[i].value)
                letterlist.push(tmLetterSet.rows[i].count)
            }
        else
            letterlist = defaults.languages[index].letters
        return letterlist
    }

    function setLetterSet(letterlist) {
        tmLetterSet.clear()
        for (var i = 0; i < letterlist.length; i += 3)
            tmLetterSet.appendRow({
                                      "letter": letterlist[i + 0],
                                      "value": parseInt(letterlist[i + 1]),
                                      "count": parseInt(letterlist[i + 2])
                                  })
        checkDefault()
        tvLetterSet.modelChanged() //update sum
    }

    function updateLetterSum() {
        var nSum = 0
        if (tmLetterSet)
            for (var i = 0; i < tmLetterSet.rowCount; i++) {
                nSum += tmLetterSet.getRow(i).count
            }
        lbNumberOfLetters.text = nSum.toString()
    }

    columns: 2
    columnSpacing: 8
    rowSpacing: 8
    height: scrollView.height - 10

    property int multiplier: 1

    TableModel {
        id: tmLetterSet
        TableModelColumn {
            display: "letter"
        }
        TableModelColumn {
            display: "value"
        }
        TableModelColumn {
            display: "count"
        }
    }

    ColorLabel {
        id: lbLetterSet
        Layout.alignment: Qt.AlignRight
        Layout.leftMargin: 8
        Layout.topMargin: 8
        text: qsTr("Letter set:")
    }
    ColorComboBox {
        id: cbLetterSet

        Layout.topMargin: 8
        Layout.minimumWidth: 100
        Layout.preferredWidth: tvLetterSet.width - sbLetterSet.width
        model: defaults.languages
        textRole: "nativeName"

        onCurrentIndexChanged: {
            if (currentIndex >= 0)
                setLetterSet(defaults.languages[currentIndex].letters)
            // setLetterSet() can change the index
            if (currentIndex >= 0) {
                sbJoker.value = defaults.languages[currentIndex].numberOfJokers
                sbPieces.value = defaults.languages[currentIndex].numberOfLetters
                sbRandoms.value = defaults.languages[currentIndex].numberOfRandoms
                rbReadingDirectionLTR.checked
                        = defaults.languages[currentIndex].readingDirection === Qt.LeftToRight
                rbReadingDirectionRTL.checked
                        = defaults.languages[currentIndex].readingDirection === Qt.RightToLeft
            }
        }
    }
    ColorLabel {
        id: lbLetterDistribution
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
        Layout.leftMargin: 8
        Layout.topMargin: horizontalHeader.height / 2 - font.pixelSize / 2
        text: qsTr("Letter distribution:")
    }
    ColumnLayout {
        id: layoutLetterSet
        HorizontalHeaderView {
            id: horizontalHeader
            syncView: tvLetterSet
            // requires +6.5(.1?), is needed because of QTBUG-115001
            resizableColumns: false
            model: [qsTr("Letter"), qsTr("Points"), qsTr("Count")]
            delegate: Rectangle {
                implicitWidth: text.implicitWidth
                implicitHeight: Math.max(horizontalHeader.height,
                                         text.implicitHeight + 4)
                color: myPalette.mid
                border.color: myPalette.midlight
                Text {
                    id: text
                    text: modelData
                    color: myPalette.windowText
                    width: parent.width
                    height: parent.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        TableView {
            id: tvLetterSet
            boundsBehavior: Flickable.StopAtBounds
            Layout.preferredWidth: contentWidth + sbLetterSet.width
            Layout.minimumHeight: 200
            Layout.fillHeight: true
            ScrollBar.vertical: ScrollBar {
                id: sbLetterSet
                policy: Qt.ScrollBarAlwaysOn
            }
            clip: true
            model: tmLetterSet
            delegate: Rectangle {
                id: rcLetterSet
                required property bool editing

                implicitWidth: 75
                implicitHeight: 20
                color: myPalette.light
                border.color: myPalette.midlight
                Text {
                    text: display
                    visible: !editing
                    padding: 2
                    color: myPalette.windowText
                    anchors.centerIn: parent
                }
                TableView.editDelegate: TextInput {
                    id: textField
                    anchors.fill: parent
                    horizontalAlignment: TextInput.AlignHCenter
                    verticalAlignment: TextInput.AlignVCenter

                    text: display
                    color: myPalette.text
                    selectionColor: myPalette.highlight
                    selectedTextColor: myPalette.highlightedText
                    Component.onCompleted: selectAll()
                    validator: RegularExpressionValidator {
                        regularExpression: (column
                                            === 0) ? /.+/ // any character, as many as possible
                                                   : /\d|[1-9][0-9]/ // either 0..9 or 1:9 followed by 0:9
                    }
                    TableView.onCommit: {
                        var idx = tvLetterSet.model.index(row, column)
                        tmLetterSet.setData(
                                    idx, "display",
                                    (column === 0) ? text : parseInt(text))
                        tvLetterSet.modelChanged()
                    }
                }
            }
            onModelChanged: {
                checkDefault()
                updateLetterSum()
            }
        }
        GridLayout {
            columns: 2
            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: 10
            ColorLabel {
                id: lbMultiplier
                text: qsTr("Multiplier:")
                Layout.alignment: Qt.AlignRight
            }
            ColorSpinBox {
                id: sbMultiplier
                Layout.preferredWidth: 75
                from: 1
                value: 1
                onValueChanged: {
                    var idx
                    var z
                    for (var i = 0; i < tmLetterSet.rowCount; i++) {
                        idx = tvLetterSet.model.index(i, 2)
                        z = tmLetterSet.getRow(i).count
                        tmLetterSet.setData(idx, "display",
                                            (z / multiplier) * value)
                    }
                    updateLetterSum()
                    multiplier = value
                }
            }
            ColorLabel {
                text: qsTr("Total number of letters:")
                Layout.alignment: Qt.AlignRight
            }
            RowLayout {
                ColorLabel {
                    id: lbNumberOfLetters
                }
                InfoTip {
                    tiptext: qsTr("The total number of tiles in the distribution excluding blanks and random letters")
                }
            }
        }
    }
    ColorLabel {
        id: lbJoker
        Layout.alignment: Qt.AlignRight
        Layout.leftMargin: 8
        text: qsTr("Blank tiles:")
    }
    RowLayout {
        ColorSpinBox {
            id: sbJoker
            value: 2
            onValueChanged: config.numberOfJokers = value
        }
        InfoTip {
            tiptext: qsTr("The number of blank tiles, also known as jokers, will be added to the letter set")
        }
    }
    ColorLabel {
        id: lbPieces
        Layout.alignment: Qt.AlignRight
        Layout.leftMargin: 8
        text: qsTr("Letters on rack:")
    }
    RowLayout {
        ColorSpinBox {
            id: sbPieces
            value: 7
            onValueChanged: config.numberOfLettersOnRack = value
        }
        InfoTip {
            tiptext: qsTr("You can define how many tiles should be available on the rack")
        }
    }
    ColorLabel {
        id: lbRandoms
        Layout.alignment: Qt.AlignRight
        Layout.leftMargin: 8
        text: qsTr("Random letters:")
    }
    RowLayout {
        ColorSpinBox {
            id: sbRandoms
            value: 0
            onValueChanged: config.numberOfRandomLetters = value
        }
        InfoTip {
            tiptext: qsTr("Random letters are picked from the distribution, also in case of zero count, and will be added to the letter set")
        }
    }
    ColorLabel {
        id: lbReadingDirection
        Layout.alignment: Qt.AlignRight
        Layout.leftMargin: 8
        text: qsTr("Reading direction:")
    }
    RowLayout {
        id: layoutReadingDirection
        ColorRadioButton {
            id: rbReadingDirectionLTR
            text: qsTr("left to right")
            onCheckedChanged: if (checked)
                                  config.ltr = true
        }
        ColorRadioButton {
            id: rbReadingDirectionRTL
            text: qsTr("right to left")
            onCheckedChanged: if (checked)
                                  config.ltr = false
        }
    }
}
