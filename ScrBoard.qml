import QtQuick
import QtQuick.Controls

Item {
    anchors.fill: parent

    property int fieldSize
    property alias jokerPicker: jokerPicker //alias needed for newgame > updatemodel

    function updateFieldSize() {
        fieldSize = height < width
                  ? height / (GamePlay.boardSize+2.5)
                  : width / (GamePlay.boardSize+1.5)
    }
    onWidthChanged: updateFieldSize();
    onHeightChanged: updateFieldSize();

    ScrJokerPicker { id: jokerPicker; visible: false }

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
        for (var i = 0; i < GamePlay.boardSize; i++)
        {
            horizLabelModel.append({"label":i+1})
            if (i < 26)
                vertLabelModel.append({"label":String.fromCharCode(65 + i)});
            else
                vertLabelModel.append({"label":String.fromCharCode(65 + i % 26) + Math.floor(i / 26)}); //A..Z,A1..Z1,A2..Z2...
        }
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
                color: config.myPalette.windowText
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
            property int aCurrentMove: GamePlay.currentMove

            implicitWidth: fieldSize
            implicitHeight: fieldSize

            pieceColor: aWhat == String.fromCharCode(0) //empty?
                        ? config.colors.get(fieldtype).itemColor //use field color when empty
                        : isPlaced && config.bColoredPlayers && (aCurrentMove - when) <= GamePlay.numberOfPlayers
                          ? Qt.lighter(config.playercolors.get(who).itemColor, 1.75) //use (lighter) player color for the last move, if set in config
                          : when === aCurrentMove
                            ? config.colors.get(8).itemColor //use (yellow) field color for non-empty squares
                            : Qt.lighter(config.colors.get(8).itemColor) // ... but a bit lighter when removing is not possible

            pieceLabel: aWhat
            pieceValue: value
            pieceIsJoker: isJoker
            pieceIsRandom: isRandom
            pieceShadow: (isPlaced && config.bColoredPlayers)
                         ? config.playercolors.get(who).itemColor //colored shadow to show who placed a piece
                         : "transparent"

            bonusTop:    config.bMarkers && (bonustop > 0)    ? config.colors.get(bonustop).itemColor   : "transparent"
            bonusBottom: config.bMarkers && (bonusbottom > 0) ? config.colors.get(bonusbottom).itemColor : "transparent"
            bonusLeft: config.ltr
                ? config.bMarkers && (bonusleft > 0) ? config.colors.get(bonusleft).itemColor  : "transparent"
                : config.bMarkers && (bonusright > 0) ? config.colors.get(bonusright).itemColor : "transparent"
            bonusRight: config.ltr
                ? config.bMarkers && (bonusright > 0) ? config.colors.get(bonusright).itemColor : "transparent"
                : config.bMarkers && (bonusleft > 0) ? config.colors.get(bonusleft).itemColor  : "transparent"

            ToolTip {
                id: tipMeaning
                visible: maBoard.containsMouse && (text !== "<html>\n</html>") && (text !== "")
                delay: 1000
                timeout: 5000
                background: Rectangle { // required on macOS
                    color: config.myPalette.ToolTipBase ? config.myPalette.ToolTipBase : "#ffffca"
                }
            }

            DropArea {
                anchors.fill: parent
                keys: "scrPiece"
                onEntered: (drag)=> {
                    rcPiece.border.color = Qt.darker(rcPiece.color)
                    drag.source.dragAccept = GamePlay.canDrop(index)
                }
                onExited: {
                    rcPiece.border.color = Qt.lighter(rcPiece.color)
                    drag.source.dragAccept = false
                }
                onDropped: (drag)=> {
                    rcPiece.border.color = "transparent"
                    GamePlay.dropLetter(drag.source.rackIndex, index)
                    if (drag.source.pieceIsJoker) {
                        var p = mapToItem(board, drag.x,drag.y)
                        if (scrabble3D.width > p.x + jokerPicker.width)
                            jokerPicker.x = p.x; else
                            jokerPicker.x = p.x - jokerPicker.width
                        if (scrabble3D.height > p.y + jokerPicker.height)
                            jokerPicker.y = p.y; else
                            jokerPicker.y = p.y - jokerPicker.height
                        jokerPicker.boardIndex = index
                        jokerPicker.open()
                    }
                }
            }
            MouseArea {
                id: maBoard
                property var rackItem
                property int rackIndex
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onEntered: tipMeaning.text = GamePlay.getMeaningAt(index)
                onPressed: (mouse)=> {
                    if (!isPlaced && (aWhat !== String.fromCharCode(0))) {
                        if (mouse.button === Qt.RightButton)
                            GamePlay.removeLetter(index);
                        if (mouse.button === Qt.LeftButton) {
                            rackIndex = GamePlay.removeLetter(index)
                            var p = mapToItem(rackList, mouseX, mouseY)
                            rackItem = rackList.itemAtIndex(rackIndex)
                            rackItem.dragStartedAt = Qt.point(rackItem.x, rackItem.y)
                            rackItem.x = p.x - fieldSize / 2
                            rackItem.y = p.y - fieldSize / 2
                            rackItem.z = Infinity
                            drag.target = rackItem
                            rackItem.Drag.active = true
                         }
                     }
                 }
                onReleased: (mouse)=> {
                    if ((mouse.button === Qt.LeftButton) &&
                        (Drag.source.rackItem !== undefined))
                        rackItem.maRack.released(mouse)
                }
            }
            TapHandler {
                onTapped: tipMeaning.visible = true
            }
        }
    }

    Component {
        id: rackDelegate
        ScrPiece {
            id: rcPiece
            property int rackIndex: index
            property alias maRack: maRack //required to call released() for backAnim

            width: fieldSize
            height: fieldSize
            border.color: Qt.darker(color)
            pieceColor: (!GamePlay.isLocalPlayer) || isExchange
                            ? config.colors.get(0).itemColor //start
                            : config.colors.get(8).itemColor //piece
            visible: isVisible //LetterState::lsRack
            pieceLabel: what
            pieceValue: value
            pieceIsJoker: isJoker
            pieceIsRandom: isRandom
            property bool dragAccept: false
            property point dragStartedAt: Qt.point(0,0)
            Drag.active: maRack.drag.active
            Drag.keys: "scrPiece"
            DropArea {
                keys: "scrPiece"
                anchors.fill: parent
                onEntered: (drag)=> { drag.source.dragAccept = true }
                onExited: drag.source.dragAccept = false
                onDropped: (drag)=> GamePlay.dropLetterRack(drag.source.rackIndex, index)
            }

            MouseArea {
                id: maRack
                anchors.fill: parent
                cursorShape: drag.active ? (dragAccept ? Qt.DragLinkCursor : Qt.ClosedHandCursor) : Qt.ArrowCursor
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onPressed: (mouse) => {
                    if (mouse.button === Qt.RightButton) {
                       rcPiece.dragStartedAt = Qt.point(0,0) //block back animation on release
                       GamePlay.exchangeLetter(index)
                    } else
                    if ((mouse.button === Qt.LeftButton) && GamePlay.canDrag()) {
                       drag.target = parent
                       rcPiece.dragStartedAt = Qt.point(rcPiece.x, rcPiece.y)
                       rcPiece.z = Infinity //topmost z-order
                    }
                }
                onPressAndHold: GamePlay.exchangeLetter(index)
                onReleased: (drag) => {
                    rcPiece.z = rackIndex //original z-order
                    if (rcPiece.dragAccept) {
                        rcPiece.Drag.drop()
                    } else
                    if (dragStartedAt !== Qt.point(0,0)) {
                        backAnimX.from = rcPiece.x;
                        backAnimX.to = dragStartedAt.x;
                        backAnimY.from = rcPiece.y;
                        backAnimY.to = dragStartedAt.y;
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

    ListView {
        id: vertLabels
        anchors.top: gdBoard.top
        anchors.left: parent.left
        height: fieldSize * GamePlay.boardSize
        width: fieldSize
        model: vertLabelModel
        delegate: labelDelegate
        orientation: ListView.Vertical
        interactive: false
    }
    ListView {
        id: horzLabels
        anchors.top: parent.top
        anchors.left: gdBoard.left
        width: fieldSize * GamePlay.boardSize
        height: fieldSize
        model: horizLabelModel
        delegate: labelDelegate
        orientation: ListView.Horizontal
        interactive: false
        layoutDirection: config.ltr ? Qt.LeftToRight : Qt.RightToLeft
        onLayoutDirectionChanged: config.ltr ? anchors.left = gdBoard.left : anchors.right = gdBoard.right
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
        interactive: false
        layoutDirection: config.ltr ? Qt.LeftToRight : Qt.RightToLeft
    }
    ListView {
        id: rackList
        anchors.left: parent.left
        anchors.leftMargin: fieldSize
        anchors.top: gdBoard.bottom
        anchors.topMargin: fieldSize * 0.25
        width: gdBoard.width
        height: fieldSize //any height works; none makes pieces invisible
        orientation: Qt.Horizontal
        spacing: 1
        model: GamePlay.rackModel
        delegate: rackDelegate
    }
}
