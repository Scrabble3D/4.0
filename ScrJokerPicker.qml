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
            pieceLabel: what
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
        for (var i=0; i<config.letterSet.rowCount; i++)
            pickerModel.append({"what":config.letterSet.getRow(i).letter,
                                "value":config.letterSet.getRow(i).value})
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
