import QtQuick
import QtQuick.Layouts

Flow {
    property int fieldSize: 20

    anchors.leftMargin: fieldSize
    anchors.fill: parent
    anchors.topMargin: (config.fieldCount + 1.2) * fieldSize
    spacing: 2

    Repeater {
        model: game.rack.length
        ScrPiece {
            property bool dragAccept: false
            property point beginDrag

            id: rcPiece
            width: fieldSize
            height: fieldSize
            border.color: Qt.darker(color)
            //random label/value
            pieceLabel: String.fromCharCode(Math.random()*20+65) //aPieces[index].What;
            pieceValue: Math.trunc(Math.random()*20)
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
                        rcPiece.visible = false
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
