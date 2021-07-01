import QtQuick
import QtQuick.Shapes

Rectangle {
    id: root
    property string pieceLabel: String.fromCharCode(0)
    property int pieceValue: 0
    property color pieceColor: "transparent"
    property color bonusTop: "transparent"
    property color bonusLeft: "transparent"
    property color bonusRight: "transparent"
    property color bonusBottom: "transparent"

    property int cMarkerSize: width/5

    color: (pieceLabel === String.fromCharCode(0)) ? pieceColor : config.colors.get(8).itemColor
    border.color: Qt.lighter(color)
//    border.color: (pieceLabel === "") ? Qt.lighter(color) : "transparent"
//                    Drag.active: maPiece.drag.active
    Text {
        text: (pieceLabel === String.fromCharCode(0)) ? "" : pieceLabel
        anchors.centerIn: parent
        font.pixelSize: parent.height*0.75
    }

    Text {
        text: (pieceValue > 0) ? pieceValue.toString() : ""
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 3
        font.pixelSize: parent.height*0.25
    }

    Rectangle {
        id: rcBonusTop
        color: "transparent"
        width: cMarkerSize; height: cMarkerSize
        anchors.left: parent.horizontalCenter; anchors.leftMargin: -cMarkerSize/2
        Shape {
            ShapePath {
               fillColor: (pieceLabel === String.fromCharCode(0)) ? bonusTop : "transparent"
               strokeColor: "transparent"
               startX:0; startY:0
               PathLine {x: rcBonusTop.width; y: 0 }
               PathLine {x: rcBonusTop.width/2; y: rcBonusTop.height }
            }
        }
    }
    Rectangle {
        id: rcBonusLeft
        color: "transparent"
        width: cMarkerSize; height: cMarkerSize
        anchors.top: parent.verticalCenter; anchors.topMargin: -cMarkerSize/2
        Shape {
            ShapePath {
               fillColor: (pieceLabel === String.fromCharCode(0)) ? bonusLeft : "transparent"
               strokeColor: "transparent"
               startX:0; startY:0
               PathLine {x: rcBonusLeft.width; y: rcBonusLeft.height/2 }
               PathLine {x: 0; y: rcBonusLeft.height }
            }
        }
    }
    Rectangle {
        id: rcBonusRight
        color: "transparent"
        width: cMarkerSize; height: cMarkerSize
        anchors.top: parent.verticalCenter; anchors.topMargin: -cMarkerSize/2; anchors.right: parent.right
        Shape {
            ShapePath {
               fillColor: (pieceLabel === String.fromCharCode(0)) ? bonusRight : "transparent"
               strokeColor: "transparent"
               startX:rcBonusRight.width; startY:0
               PathLine {x: 0; y: rcBonusRight.height/2 }
               PathLine {x: rcBonusRight.width; y: rcBonusRight.height }
            }
        }
    }
    Rectangle {
        id: rcBonusBottom
        color: "transparent"
        width: cMarkerSize; height: cMarkerSize
        anchors.left: parent.horizontalCenter; anchors.leftMargin: -cMarkerSize/2; anchors.bottom: parent.bottom
        Shape {
            ShapePath {
               fillColor: (pieceLabel === String.fromCharCode(0)) ? bonusBottom : "transparent"
               strokeColor: "transparent"
               startX:0; startY:rcBonusBottom.height
               PathLine {x: rcBonusBottom.width; y: rcBonusBottom.height }
               PathLine {x: rcBonusBottom.width/2; y: 0 }
            }
        }
    }

}

