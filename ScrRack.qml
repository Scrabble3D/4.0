import QtQuick
import QtQuick.Layouts
import letterstate

Flow {
    property int fieldSize: 20

    anchors.leftMargin: fieldSize
    anchors.fill: parent
    anchors.topMargin: (GamePlay.boardSize + 1.2) * fieldSize
    spacing: 2
//    GamePlay.rackSizeChanged: console.log("size changed");
//    GamePlay.rackChanged: console.log("rack changed");
    Repeater {
        model: GamePlay.rackSize
        ScrPiece {
            property bool dragAccept: false
            property point beginDrag
//            GamePlay.setRackIndex: index
            id: rcPiece
            width: fieldSize
            height: fieldSize
            //cannot read - when not defined
            //visible: GamePlay.rack[index].state === LetterState.lsRack
            border.color: Qt.darker(color)
            pieceLabel: GamePlay.rack[index].what
            pieceValue: GamePlay.rack[index].value
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
//                        GamePlay.placeLetter(index,0)
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
