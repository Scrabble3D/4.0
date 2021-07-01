import QtQuick
import QtQuick.Layouts

GridLayout {

    property int fieldSize: 20 //px
/*
    function newGame() {
        for (var i=0; i<rcBoard.count; i++) {
            rcSquare[i].
        }
    }
*/
    rows: game.boardSize+1
    columns: game.boardSize+1
    rowSpacing: 0
    columnSpacing: 0
    antialiasing: true
    Repeater {
        id: lbHorizontal
        model: game.boardSize
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
        model: game.boardSize
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
        model: game.boardSize * game.boardSize
        ScrPiece {
            id: rcSquare
            Layout.column: (index % game.boardSize) + 1
            Layout.row: Math.floor(index / game.boardSize) + 1
            Layout.preferredHeight: fieldSize
            Layout.preferredWidth: fieldSize
            pieceColor:  config.colors.get(game.fields[index]).itemColor
            bonusTop:    config.markers && (index-game.boardSize>0) && (game.fields[index-game.boardSize]>1) ?
                             config.colors.get(game.fields[index-game.boardSize]).itemColor : "transparent"
            bonusLeft:   config.markers && (index % game.boardSize)>0 && (game.fields[index-1]>1) ?
                             config.colors.get(game.fields[index-1]).itemColor : "transparent"
            bonusRight:  config.markers && (index+1 % game.boardSize)>0 && (game.fields[index+1]>1) ?
                             config.colors.get(game.fields[index+1]).itemColor : "transparent"
            bonusBottom: config.markers && (index+game.boardSize<game.fields.length) && (game.fields[index+game.fieldCount]>1) ?
                             config.colors.get(game.fields[index+game.boardSize]).itemColor : "transparent"

            DropArea {
                anchors.fill: parent
                onEntered: drag.source.dragAccept = (game.getLetter(index)[0] === String.fromCharCode(0))
//                onExited: drag.source.dragAccept = false
                onDropped: {
                    if (drag.source.dragAccept)
                    {
                        game.setLetter(drag.source.pieceLabel, drag.source.pieceValue, game.currentMove, index)
                        rcSquare.pieceLabel = game.getLetter(index)[0]
                        rcSquare.pieceValue = game.getLetter(index)[1]
                        game.checkMove()
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
                        pieceLabel = String.fromCharCode(0)
                        pieceValue = 0
                        game.setLetter(String.fromCharCode(0), 0, 0, index)
                        game.checkMove()
                    }
                }
                onReleased: {
                }
//                        drag.target: rcSquares
//                        onReleased: game.letters[index] = rcPieces.Text
            }
        }
    }
}
