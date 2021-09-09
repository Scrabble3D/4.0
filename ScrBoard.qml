import QtQuick
import QtQuick.Layouts

GridLayout {

    property int fieldSize: 20 //px
    property int boardSize: GamePlay.boardSize
/*
    function newGame() {
        for (var i=0; i<rcBoard.count; i++) {
            rcSquare[i].
        }
    }
*/
    rows: boardSize+1
    columns: boardSize+1
    rowSpacing: 0
    columnSpacing: 0
    antialiasing: true
    Repeater {
        id: lbHorizontal
        model: boardSize
        Rectangle {
            Layout.row: 0
            Layout.column: index+1
            Layout.preferredWidth: fieldSize
            Layout.preferredHeight: fieldSize
            Text {
                text: index+1
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 2
            }
        } //Label
    }
    Repeater {
        id: lbVertical
        model: boardSize
        Rectangle {
            Layout.row: index+1
            Layout.column: 0
            Layout.preferredWidth: fieldSize
            Layout.preferredHeight: fieldSize
            Text {
                //TODO: 3D dimension
                text: index < 26 ? String.fromCharCode(65 + index)
                                 : String.fromCharCode(65 + index % 26) + Math.floor(index / 26)
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 5
            }
        } //Label
    }
    Repeater {
        id: rcBoard
        model: boardSize * boardSize
        ScrPiece {
            id: rcSquare
            Layout.column: (index % boardSize) + 1
            Layout.row: Math.floor(index / boardSize) + 1
            Layout.preferredHeight: fieldSize
            Layout.preferredWidth: fieldSize
            pieceColor:  config.colors.get(GamePlay.fieldType(index)).itemColor
            pieceLabel: GamePlay.boardLetter(index).what
            pieceValue: GamePlay.boardLetter(index).value
            bonusTop:    config.markers && (index-boardSize>0) && (GamePlay.fieldType(index-boardSize)>1) ?
                             config.colors.get(GamePlay.fieldType(index-boardSize)).itemColor : "transparent"
            bonusLeft:   config.markers && (index % boardSize)>0 && (GamePlay.fieldType(index-1)>1) ?
                             config.colors.get(GamePlay.fieldType(index-1)).itemColor : "transparent"
            bonusRight:  config.markers && (index+1 % boardSize)>0 && (GamePlay.fieldType(index+1)>1) ?
                             config.colors.get(GamePlay.fieldType(index+1)).itemColor : "transparent"
            bonusBottom: config.markers && (index+boardSize<boardSize*boardSize) && (GamePlay.fieldType(index+boardSize*boardSize)>1) ?
                             config.colors.get(GamePlay.fieldType(index+boardSize)).itemColor : "transparent"

            DropArea {
                anchors.fill: parent
                onEntered: drag.source.dragAccept = GamePlay.canDrop(index);
//                           (game.getLetter(index)[0] === String.fromCharCode(0))
//                onExited: drag.source.dragAccept = false
                onDropped: {
                    if (drag.source.dragAccept)
                    {
                        GamePlay.placeLetter(drag.source.rackIndex, index)
//                        game.setLetter(drag.source.pieceLabel, drag.source.pieceValue, game.currentMove, index)
 //                        rcSquare.pieceLabel = GamePlay.boardLetter(index).what
 //                        rcSquare.pieceValue = GamePlay.boardLetter(index).value
//                        game.checkMove()
//                        rcSquare.state = "new"
                    }
                }
            }

            MouseArea {
                id: maSquare
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    if (mouse.button === Qt.LeftButton) {
                        //TODO: drag 'n drop
                    } else
                    {
                        GamePlay.removeLetter(index);
/*                        pieceLabel = String.fromCharCode(0)
                        pieceValue = 0
//                        game.rack.append({"letter":pieceLabel,"value":pieceValue,""})
                        game.setLetter(String.fromCharCode(0), 0, 0, index)
                        game.checkMove()
*/                    }
                }
                onReleased: {
                }
//                        drag.target: rcSquares
//                        onReleased: game.letters[index] = rcPieces.Text
            }
        }
    }
}
