import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    columns: 2
    columnSpacing: 8
    rowSpacing: 8

    property alias cbLetterSet: cbLetterSet
    property alias sbJoker: sbJoker
    property alias sbPieces: sbPieces
    property alias sbRandoms: sbRandoms
    property alias rbReadingDirectionLTR: rbReadingDirectionLTR
    property alias rbReadingDirectionRTL: rbReadingDirectionRTL

    property string lettersetName: cbLetterSet.currentText

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
            Layout.minimumWidth: 200
            Layout.preferredWidth: 300
            model: defaults.languages
            textRole: "nativeName"
            onCurrentIndexChanged: if ( currentIndex >= 0 ) {
                config.letterSet = defaults.languages[currentIndex].letters
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
        //TODO: configletter: check fillHeight does not work?
        Layout.fillHeight: true
        HorizontalHeaderView {
            id: horizontalHeader
            syncView: tvLetterSet
            implicitWidth: syncView ? syncView.width : 0
            implicitHeight: contentHeight

            model: [qsTr("Letter"), qsTr("Points"), qsTr("Count")]
            delegate: Rectangle {
                implicitWidth: text.implicitWidth + 4
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
        onValueChanged: config.numberOfJokers = value
    }
    Label {
        id: lbPieces
        Layout.leftMargin: 8
        Layout.alignment: Qt.AlignRight
        text: qsTr("Number of letters on rack:")
    }
    SpinBox {
        id: sbPieces
        onValueChanged: config.numberOfLettersOnRack = value
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
