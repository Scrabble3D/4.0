import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    property int boardIndex

    title: qsTr("Pick a letter")
    width: contentWidth + 28
    height: contentHeight + 64
    modal: true
    onRejected: GamePlay.removeLetter(boardIndex)
    Component {
        id: pickerItem
        ScrPiece {
            implicitWidth: 48
            implicitHeight: 48
            pieceLabel: GamePlay.replaceDigraph(what)
            pieceColor: config.colors.get(8).itemColor //yellow
            pieceValue: value
            border.width: 1
            border.color: "lightgrey"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    GamePlay.setJokerLetter(boardIndex, pickerModel.get(index).what)
                    close()
                }
            }
        }
    }
    ListModel {
        id: pickerModel
    }
    function updatePickerModel() {
        pickerModel.clear()
        var letterlist = config.getLetterSet(-1)
        for (var i = 0; i < letterlist.length; i+=3)
            pickerModel.append( {"what"  : letterlist[i+0],
                                 "value" : letterlist[i+1]} )
    }
    Grid {
        columns: Math.max(Math.sqrt(pickerModel.count), 6)
        spacing: 1
        Repeater {
            id: picker
            model: pickerModel
            delegate: pickerItem
        }
    }
}
