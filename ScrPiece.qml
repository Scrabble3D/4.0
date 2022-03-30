import QtQuick
import QtQuick.Shapes

Rectangle {
    property string pieceLabel: String.fromCharCode(0)
    property int pieceValue: 0
    property bool pieceIsJoker: false
    property color pieceColor: "transparent"
    property color pieceShadow: "transparent"
    property color bonusTop: "transparent"
    property color bonusLeft: "transparent"
    property color bonusRight: "transparent"
    property color bonusBottom: "transparent"

    property int cMarkerSize: width/5

    color: pieceColor

    border.color: Qt.lighter(color)
//    border.color: (pieceLabel === "") ? Qt.lighter(color) : "transparent"

    Text {
        text: (pieceLabel === String.fromCharCode(0) || pieceLabel === "?") ? "" : pieceLabel
        color: pieceShadow
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 1
        anchors.horizontalCenterOffset: 1
        font.pixelSize: parent.height*0.75
    }
    Text {
        text: (pieceLabel === String.fromCharCode(0) || pieceLabel === "?") ? "" : pieceLabel
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

    Image {
        visible: pieceIsJoker
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 3
        anchors.bottomMargin: 3
        width: parent.height*0.25
        height: parent.height*0.25
        source: "qrc:///resources/star.png"
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

