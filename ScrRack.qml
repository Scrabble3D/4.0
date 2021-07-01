import QtQuick
import QtQuick.Layouts

Flow {
    property int fieldSize: 20

    anchors.leftMargin: fieldSize
    anchors.fill: parent
    anchors.topMargin: (game.boardSize + 1.2) * fieldSize
    spacing: 2

    Repeater {
        model: game.rack.count
        ScrPiece {
            property bool dragAccept: false
            property point beginDrag

            id: rcPiece
            width: fieldSize
            height: fieldSize
            border.color: Qt.darker(color)
            //random label/value
            pieceLabel: game.rack.get(index).letter //String.fromCharCode(Math.random()*20+65) //aPieces[index].What;
            pieceValue: game.rack.get(index).value //Math.trunc(Math.random()*20)
            Drag.active: maPiece.drag.active
            MouseArea {
                id: maPiece
                anchors.fill: parent
                drag.target: parent //rcBoard.rcSquares
                cursorShape: drag.active ? (dragAccept ? Qt.DragLinkCursor : Qt.ClosedHandCursor) : Qt.ArrowCursor
                onPressed: {
                    rcPiece.beginDrag = Qt.point(rcPiece.x, rcPiece.y);
                }
                onReleased: {
                    if (dragAccept)
                    {
                        game.rack.remove(index)
                        parent.Drag.drop()
                    } else
                    {
                        backAnimX.from = rcPiece.x;
                        backAnimX.to = beginDrag.x;
                        backAnimY.from = rcPiece.y;
                        backAnimY.to = beginDrag.y;
                        backAnim.start()
                    }
                }
            }
            ParallelAnimation {
                id: backAnim
                SpringAnimation { id: backAnimX; target: rcPiece; property: "x";
                                  duration: 500; spring: 2; damping: 0.2 }
                SpringAnimation { id: backAnimY; target: rcPiece; property: "y";
                                  duration: 500; spring: 2; damping: 0.2 }
            }
        }
    }

}
