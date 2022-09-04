import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    title: qsTr("Search for words")
    standardButtons: Dialog.Close
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    property int nIndex: -1

    property var dicEntry
    onDicEntryChanged: {
        dicWord.text = dicEntry["word"]
        dicCategory.text = dicEntry["category"]
        dicMeaning.text = dicEntry["meaning"]
        dicWord.bgcolor = dicEntry["included"]
                ? config.myPalette.light : "yellow"
        dicWord.color = dicEntry["included"]
                ? palette.text : "black"
    }

    function isWordInDic(sWord) {
        var index = GamePlay.indexByWord(sWord)
        if (index > -1) {
            if (dicIndex.value == index+1)
                dicIndex.valueChanged() //just emit signal if the value hasn't changed
            else
                dicIndex.value = index+1
        } else {
            dicCategory.text = ""
            dicMeaning.text = ""
            dicWord.bgcolor = "red"
            dicWord.color = "black"
        }
    }
    onAboutToShow: dicIndex.valueChanged() //update in case the category is not included anymore

    RowLayout {
        GridLayout {
            id: glLeft
            columns: 2
            rowSpacing: 8
            Layout.margins: 6
            Label {
                text: qsTr("Word:")
            }
            //TODO: wordsearch: digraph replacement (Espanol -> 1ACA = LLACA)
            TextField {
                id: dicWord
                property string bgcolor: config.myPalette.light
                Layout.preferredWidth: 150
                Layout.preferredHeight: font.pixelSize + 10
                verticalAlignment: Text.AlignVCenter
                font.capitalization: Font.AllUppercase
                leftPadding: 3
                background: Rectangle {
                    anchors.fill: parent
                    color: parent.bgcolor
                    border.color: config.myPalette.mid
                }
                onTextChanged: isWordInDic(dicWord.text)
            }
            Label {
                text: qsTr("Number:")
            }
            SpinBox {
                id: dicIndex
                Layout.fillWidth: true
                from: 1
                to: GamePlay.dicWordCount
                onValueChanged: dicEntry = GamePlay.wordByIndex(value-1)
            }
            Label {
                text: qsTr("Category:")
            }
            Label {
                id: dicCategory
                Layout.preferredWidth: 150
                elide: Text.ElideRight
            }
            Label {
                Layout.alignment: Text.AlignTop
                text: qsTr("Meaning:")
            }
            Label {
                id: dicMeaning
                Layout.preferredHeight: 100
                Layout.preferredWidth: 150
                wrapMode: Text.Wrap
                verticalAlignment: Text.AlignTop
            }
        }
        GridLayout {
            id: glRight
            columns: 2
            rowSpacing: 8
            Layout.margins: 6
            Layout.alignment: Qt.AlignTop
            Label { text: qsTr("Pattern:") }
            TextField {
                id: dicPattern
                Layout.preferredWidth: 150
                Layout.preferredHeight: font.pixelSize + 10
                text: ""
                verticalAlignment: Text.AlignVCenter
                font.capitalization: Font.AllUppercase
                leftPadding: 3
                background: Rectangle {
                    anchors.fill: parent
                    border.color: config.myPalette.mid
                    color: config.myPalette.light
                }
                color: palette.windowText
                onTextChanged: {
                    var aWords = GamePlay.getVariation(dicPattern.text).split(",")
                    dicSearchModel.clear()
                    for (var i=0; i<aWords.length; i++)
                        dicSearchModel.append({"word":aWords[i]})
                }
            }

            ListModel { id: dicSearchModel }
            Label {
                Layout.alignment: Qt.AlignTop
                text: qsTr("Result:") }
            ListView {
                id: dicSearchResult
                Layout.fillHeight: true
                Layout.fillWidth: true
                model: dicSearchModel
                clip: true
                delegate: Rectangle {
                    height: delegateText.font.pixelSize + 8
                    width: dicPattern.width
                    //TODO: ScrWordSearch: palette at search not working with dark themes
                    color: ListView.isCurrentItem ? config.myPalette.highlight : config.myPalette.window
                    Text {
                        id: delegateText
                        text: word
                        anchors.fill: parent
                        leftPadding: 3
                        verticalAlignment: Text.AlignVCenter
                        color: dicSearchResult.currentIndex == index ? config.myPalette.highlightedText : config.myPalette.windowText
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            dicWord.text = delegateText.text
                            dicSearchResult.currentIndex = index
                        }
                    }
                }
                ScrollBar.vertical: ScrollBar {}
            }
        }
    }
}
