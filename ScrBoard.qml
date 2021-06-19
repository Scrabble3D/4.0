import QtQuick
import QtQuick.Layouts

GridLayout {

    property int fieldSize: 20

    rows: config.fieldCount+1
    columns: config.fieldCount+1
    rowSpacing: 0
    columnSpacing: 0
    antialiasing: true
    Repeater {
        id: lbHorizontal
        model: config.fieldCount
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
        model: config.fieldCount
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
        model: config.fieldCount * config.fieldCount
        ScrPiece {
            id: rcSquares
            Layout.column: (index % config.fieldCount) + 1
            Layout.row: Math.floor(index / config.fieldCount) + 1
            Layout.preferredHeight: fieldSize
            Layout.preferredWidth: fieldSize
            pieceColor:  config.colors.get(config.fieldType[index]).itemColor
            bonusTop:    config.markers && (index-config.fieldCount>0) && (config.fieldType[index-config.fieldCount]>1) ?
                             config.colors.get(config.fieldType[index-config.fieldCount]).itemColor : "transparent"
            bonusLeft:   config.markers && (index % config.fieldCount)>0 && (config.fieldType[index-1]>1) ?
                             config.colors.get(config.fieldType[index-1]).itemColor : "transparent"
            bonusRight:  config.markers && (index+1 % config.fieldCount)>0 && (config.fieldType[index+1]>1) ?
                             config.colors.get(config.fieldType[index+1]).itemColor : "transparent"
            bonusBottom: config.markers && (index+config.fieldCount<config.fieldType.length) && (config.fieldType[index+config.fieldCount]>1) ?
                             config.colors.get(config.fieldType[index+config.fieldCount]).itemColor : "transparent"
            DropArea {
                id: dragTarget
                anchors.fill: parent
                property bool dropped: false
                onEntered:
                    drag.source.dragAccept = (game.letters[index] === "")
                onDropped: {
                    if (drag.source.dragAccept)
                    {
                        game.letters[index] = drag.source.pieceLabel
                        rcSquares.pieceLabel = game.letters[index]
                        dropped = true
                    }
                }
            }

            MouseArea {
                id: maSquare
                anchors.fill: parent
                onClicked: {
//                            if (mouse.button === Qt.RightButton)
                        console.log("foo")
                }
                onReleased: {
                }
//                        drag.target: rcSquares
//                        onReleased: game.letters[index] = rcPieces.Text
            }
        }
    }
}
