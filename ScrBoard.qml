import QtQuick
import QtQuick.Layouts

Item {
    anchors.fill: parent

    property int fieldSize //set in ScrMain

    ListModel {
        id: horizLabelModel
        dynamicRoles: true
    }
    ListModel {
        id: vertLabelModel
        dynamicRoles: true
    }
    Component.onCompleted: updateLabelsModel();

    function updateLabelsModel() {
        horizLabelModel.clear()
        vertLabelModel.clear()
        horizLabelModel.append({"label":""}); //empty dummy for the top left corner
        vertLabelModel.append({"label":""});
        for (var i=0; i<GamePlay.boardSize; i++)
        {
            horizLabelModel.append({"label":i+1})
            if (i < 26)
                vertLabelModel.append({"label":String.fromCharCode(65 + i)});
            else
                vertLabelModel.append({"label":String.fromCharCode(65 + i % 26) + Math.floor(i / 26)});
        }
    }

    ListView {
        id: horzLabels
        anchors.top: parent.top
        anchors.left: parent.left
        width: fieldSize * GamePlay.boardSize
        height: fieldSize
        model: horizLabelModel
        delegate: labelDelegate
        orientation: ListView.Horizontal
    }
    ListView {
        id: vertLabels
        anchors.top: parent.top
        anchors.left: parent.left
        height: fieldSize * GamePlay.boardSize
        model: vertLabelModel
        delegate: labelDelegate
        orientation: ListView.Vertical
    }
    GridView {
        id: gdBoard
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: fieldSize
        anchors.leftMargin: fieldSize
        width: fieldSize*GamePlay.boardSize
        height: fieldSize*GamePlay.boardSize
        cellHeight: fieldSize
        cellWidth: fieldSize
        model: GamePlay.boardModel
        delegate: boardDelegate
    }

    Component {
        id: labelDelegate
        Rectangle {
            implicitWidth: fieldSize
            implicitHeight: fieldSize
            color: "transparent"
            border.color: Qt.lighter(color)
            Text {
                text: label
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    Component {
        id: boardDelegate
        ScrPiece {
            id: rcPiece
            property string aWhat: what

            implicitWidth: fieldSize
            implicitHeight: fieldSize

            function setAlpha(aColor, aAlpha) {
                return Qt.hsla(aColor.hslHue, aColor.hslSaturation, aColor.hslLightness, aAlpha)
            }

            pieceColor: aWhat == String.fromCharCode(0) //empty?
                        ? config.colors.get(fieldtype).itemColor //use field color when empty
                        : isplaced && config.colorplayers && (GamePlay.currentMove-when-1) < GamePlay.numberOfPlayers
                          ? setAlpha(config.playercolors.get(who).itemColor,0.25) //use (lighter) player color for the last move, if set in config
                          : when === GamePlay.currentMove
                            ? config.colors.get(8).itemColor //use (yellow) field color for non-empty squares
                            : Qt.lighter(config.colors.get(8).itemColor) // ... but a bit lighter when removing is not possible
            pieceLabel: aWhat
            pieceValue: value
            pieceShadow: (isplaced && config.colorplayers)
                         ? config.playercolors.get(who).itemColor //colored shadow to show who placed a piece
                         : "transparent"

            bonusTop:    config.markers && (bonustop>0)    ? config.colors.get(bonustop).itemColor   : "transparent"
            bonusLeft:   config.markers && (bonusleft>0)   ? config.colors.get(bonusleft).itemColor  : "transparent"
            bonusRight:  config.markers && (bonusright>0)  ? config.colors.get(bonusright).itemColor : "transparent"
            bonusBottom: config.markers && (bonusbottom>0) ? config.colors.get(bonusbottom).itemColor : "transparent"

            property bool dragAccept: false
            property point beginDragAt

//            Drag.active: maBoard.drag.active

            DropArea {
                anchors.fill: parent
                onEntered: drag.source.dragAccept = GamePlay.canDrop(index);
                // todo
                //onExited: drag.source.dragAccept = false
                onDropped: GamePlay.dropLetter(drag.source.rackIndex, index)
            }

            MouseArea {
                id: maBoard
                anchors.fill: parent
//                drag.target: parent
                cursorShape: drag.active ? (dragAccept ? Qt.DragLinkCursor : Qt.ClosedHandCursor) : Qt.ArrowCursor
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onPressed: if (!isplaced) {
                    if (mouse.button == Qt.RightButton)
                        GamePlay.removeLetter(index, false);
                    if (mouse.button == Qt.LeftButton)
                        if (GamePlay.removeLetter(index, true)) {
                            rcPiece.z = Infinity //BringToFront
                            rcPiece.beginDragAt = Qt.point(index, rcPiece.z);
                        }
                }
                onReleased: if (drag.active) {
                    rcPiece.z = rcPiece.beginDragAt.y //BringToFront
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
