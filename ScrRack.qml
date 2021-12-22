import QtQuick
import QtQuick.Layouts

Item {

    property int fieldSize

    ListView {
        anchors.fill: parent
        orientation: Qt.Horizontal
        spacing: 1
        model: GamePlay.rackModel
        delegate: rackDelegate
    }

    Component {
        id: rackDelegate
        ScrPiece {
            id: rcPiece
            property int rackIndex: index

            width: fieldSize
            height: fieldSize
            border.color: Qt.darker(color)
            pieceColor: config.colors.get(8).itemColor

            visible: isVisible // LetterState::lsRack
            pieceLabel: what
            pieceValue: value

            property bool dragAccept: false
            property point beginDragAt

            Drag.active: maRack.drag.active

            MouseArea {
                id: maRack
                anchors.fill: parent
                drag.target: parent
                cursorShape: drag.active ? (dragAccept ? Qt.DragLinkCursor : Qt.ClosedHandCursor) : Qt.ArrowCursor
                onPressed: {
                    rcPiece.beginDragAt = Qt.point(rcPiece.x, rcPiece.y);
                    rcPiece.z = Infinity //BringToFront
                }
                onReleased: {
                    rcPiece.z = rackIndex
                    if (dragAccept) {
                        rcPiece.Drag.drop()
                    } else
                    {
                        backAnimX.from = rcPiece.x;
                        backAnimX.to = beginDragAt.x;
                        backAnimY.from = rcPiece.y;
                        backAnimY.to = beginDragAt.y;
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
