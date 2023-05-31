import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    title: qsTr("Search for words")
    standardButtons: Dialog.Close
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 350
    height: 400

    property int nIndex: -1

    property var dicEntry
    onDicEntryChanged: {
        dicWord.text = dicEntry["word"]
        dicCategory.text = dicEntry["category"]
        dicMeaning.text = dicEntry["meaning"]
        dicWord.bgcolor = dicEntry["included"] ? palette.window : "yellow"
        dicWord.color = dicEntry["included"]   ? palette.windowText : "black"
    }

    function isWordInDic(sWord) {
        var index = GamePlay.indexByWord(sWord)
        if (index > -1) {
            if (dicIndex.value === index+1)
                dicIndex.valueChanged() //just emit signal if the value hasn't changed
            else
                dicIndex.value = index+1
        } else {
            dicCategory.text = ""
            dicMeaning.text = ""
            dicWord.bgcolor = "red"
            dicWord.color = "white"
        }
    }
    onAboutToShow: dicIndex.valueChanged() //update in case the category is not included anymore

    ColumnLayout {
        anchors.fill: parent
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            TabButton { text: qsTr("Search") }
            TabButton { text: qsTr("Pattern") }
        }
        StackLayout {
            currentIndex: tabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            GridLayout {
                id: glSearch
                columns: 2
                rowSpacing: 8
                Label { text: qsTr("Word:") }
                //NOTE: wordsearch: digraph replacement (Espanol -> 1ACA = LLACA)
                TextField {
                    id: dicWord
                    property string bgcolor: palette.light
                    Layout.preferredWidth: 200
                    verticalAlignment: Text.AlignVCenter
                    font.capitalization: Font.AllUppercase
                    leftPadding: 3
                    placeholderText: qsTr("Enter word")
                    background: Rectangle {
                        implicitHeight: dicIndex.height
                        color: parent.bgcolor
                        border.color: palette.mid
                    }
                    onTextChanged: isWordInDic(dicWord.text)
                }

                Label { text: qsTr("Number:") }
                SpinBox {
                    id: dicIndex
                    Layout.minimumWidth: 100
                    from: Math.min(to, 1) // 0 if dictionary is empty otherwise 1
                    to: GamePlay.wordCount
                    onValueChanged: if (value > 0) dicEntry = GamePlay.wordByIndex(value-1)
                }

                Label {
                    text: qsTr("Category:")
                    Layout.alignment: Text.AlignTop
                }
                Label {
                    id: dicCategory
                    wrapMode: Text.WordWrap
                    elide: Text.ElideRight
                }

                Label {
                    text: qsTr("Meaning:")
                    Layout.alignment: Text.AlignTop
                }
                Label {
                    id: dicMeaning
                    Layout.preferredWidth: 200
                    wrapMode: Text.Wrap
                    verticalAlignment: Text.AlignTop
                }
            }

            GridLayout {
                id: glPattern
                columns: 2
                rowSpacing: 8

                Label { text: qsTr("Pattern:") }
                TextField {
                    id: dicPattern
                    Layout.preferredWidth: 200
                    verticalAlignment: Text.AlignVCenter
                    font.capitalization: Font.AllUppercase
                    leftPadding: 3
                    placeholderText: qsTr("Enter letters")
                    enabled: dicIndex.to > 0 // empty dictionary
                    onTextChanged: {
                        var aWords = GamePlay.getVariation(dicPattern.text).split(",")
                        dicSearchModel.clear()
                        for (var i = 0; i < aWords.length; i++)
                            dicSearchModel.append({"word":aWords[i]})
                    }
                }

                ListModel { id: dicSearchModel }
                Label {
                    text: qsTr("Result:")
                    Layout.alignment: Qt.AlignTop
                }
                ListView {
                    id: dicSearchResult
                    Layout.fillHeight: true
                    Layout.preferredWidth: 200
                    model: dicSearchModel
                    clip: true
                    delegate: Rectangle {
                        height: delegateText.font.pixelSize + 8
                        width: dicPattern.width
                        // NOTE: ScrWordSearch: palette at search not working with dark themes on Android
                        color: ListView.isCurrentItem ? palette.highlight : palette.window
                        Text {
                            id: delegateText
                            text: word
                            anchors.fill: parent
                            leftPadding: 3
                            verticalAlignment: Text.AlignVCenter
                            color: dicSearchResult.currentIndex === index
                                   ? palette.highlightedText : palette.windowText
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
}
