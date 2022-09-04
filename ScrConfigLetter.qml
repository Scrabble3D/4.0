import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    property alias cbLetterSet: cbLetterSet
    property alias sbJoker: sbJoker
    property alias sbPieces: sbPieces
    property alias sbRandoms: sbRandoms
    property alias rbReadingDirectionLTR: rbReadingDirectionLTR
    property alias rbReadingDirectionRTL: rbReadingDirectionRTL

    function getLetterSetName() {
        var sResult = cbLetterSet.currentText
        if (sResult === "") sResult = qsTr("Special")
        return sResult
    }
    //FIXME!: configletter: if letterset are identical eg English US/GB the first one is taken ending up in a loop
    function checkDefault() {
        var comp = JSON.stringify(config.getLetterSet(-1))
        for (var i=0; i<defaults.languages.length; i++) {
           if (comp === JSON.stringify(config.getLetterSet(i))) {
               cbLetterSet.currentIndex = i
               return
           }
        }
        cbLetterSet.currentIndex = -1
    }

    columns: 2
    columnSpacing: 8
    rowSpacing: 8

    width: scrollView.width - 10
    height: scrollView.height - 10

    Label {
        id: lbLetterSet
        Layout.alignment: Qt.AlignRight
        text: qsTr("Letter set:")
    }
    ComboBox {
        id: cbLetterSet
        Layout.preferredWidth: tvLetterSet.width - sbLetterSet.width
        model: defaults.languages
        textRole: "nativeName"
        onCurrentIndexChanged:
            if ( currentIndex >= 0 ) {
                config.letterSet = defaults.languages[currentIndex].letters
                sbJoker.value = defaults.languages[currentIndex].numberOfJokers
                sbPieces.value = defaults.languages[currentIndex].numberOfLetters
                sbRandoms.value = defaults.languages[currentIndex].numberOfRandoms
                rbReadingDirectionLTR.checked = defaults.languages[currentIndex].readingDirection === Qt.LeftToRight
                rbReadingDirectionRTL.checked = defaults.languages[currentIndex].readingDirection === Qt.RightToLeft
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
        HorizontalHeaderView {
            id: horizontalHeader
            syncView: tvLetterSet
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
                    width: parent.width
                    height: parent.height
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
        TableView {
            //TODO: configletter: make letterset editable
            id: tvLetterSet
            Layout.preferredWidth: contentWidth + sbLetterSet.width
            Layout.minimumHeight: 200
            Layout.fillHeight: true
            ScrollBar.vertical: ScrollBar { id: sbLetterSet }
            clip: true
            model: config.letterSet
            delegate: Rectangle {
                implicitWidth: 75
                implicitHeight: 20
                color: myPalette.light
                border.color: myPalette.midlight
                Text {
                    text: display
                    padding: 2
                    color: myPalette.windowText
                    anchors.centerIn: parent
                }
            }
            onModelChanged: {
                checkDefault()
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
        Layout.alignment: Qt.AlignRight
        text: qsTr("Number of blank tiles:")
    }
    SpinBox {
        id: sbJoker
        onValueChanged: config.numberOfJokers = value
    }
    Label {
        id: lbPieces
        Layout.alignment: Qt.AlignRight
        text: qsTr("Number of letters on rack:")
    }
    SpinBox {
        id: sbPieces
        onValueChanged: config.numberOfLettersOnRack = value
    }
    Label {
        id: lbRandoms
        Layout.alignment: Qt.AlignRight
        text: qsTr("Number of random letters:")
    }
    SpinBox {
        id: sbRandoms
        onValueChanged: config.numberOfRandomLetters = value
    }
    Label {
        id: lbReadingDirection
        Layout.alignment: Qt.AlignRight
        text: qsTr("Reading direction:")
    }
    RowLayout {
        id: layoutReadingDirection
        RadioButton {
            id: rbReadingDirectionLTR
            text: qsTr("left to right")
        }
        RadioButton {
            id: rbReadingDirectionRTL
            text: qsTr("right to left")
        }
    }
}
