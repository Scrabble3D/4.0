import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts

ColumnLayout {
    Layout.fillHeight: true
    Layout.fillWidth: true

    //required for height of table header and rows
    FontMetrics {
        id: fontMetrics
    }

    //use 20px for first column and calculate 0 in respect to parent
    property var colwidths: [20,0,0,0,0]

    SplitView {
        id: statHeader
        Layout.fillWidth: true

        implicitHeight: fontMetrics.height + 6
        Repeater {
            id: headerColumn
            model: [qsTr("#"),qsTr("Word"),qsTr("Value"),qsTr("Best"),qsTr("Time")]
            Item {
                SplitView.minimumWidth: text.implicitWidth
                SplitView.preferredWidth: (colwidths[index] === 0) ? parent.width / 5 - 20 : colwidths[index]
                SplitView.fillWidth: (index === 1) ? true : false //expand the "Word" column
                implicitHeight: text.implicitHeight + 2
                clip: true

                Rectangle {
                    anchors.fill: parent
                    color: config.myPalette.button
                    Text {
                        id: text
                        text: modelData
                        width: parent.width
                        onWidthChanged: {
                            colwidths[index] = width
                            statTable.forceLayout()
                        }
                        height: parent.height
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: (index === 1) ? Text.AlignLeft : Text.AlignRight
                        leftPadding: 5
                        rightPadding: 5
                        color: config.myPalette.windowText
                    }
                }
            }
        }
    }

    TableView {
        id: statTable
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true
        columnWidthProvider: function(column) { return colwidths[column] } //follows the statHeader column width
        ScrollBar.vertical: ScrollBar { }
        model: GamePlay.gamecourseModel
        columnSpacing: 1
        rowSpacing: 1

        Keys.onPressed: (event)=> {
            if (event.key === Qt.Key_Down)
                GamePlay.gamecourseModel.selectedMove++;
            else if (event.key === Qt.Key_Up)
                GamePlay.gamecourseModel.selectedMove--;
            positionViewAtRow(GamePlay.gamecourseModel.selectedMove, TableView.Contain | TableView.AlignVCenter)
            if (GamePlay.is3D)
                cube.roll()
        }
        delegate: Rectangle {
            implicitWidth: parent.width
            implicitHeight: (connectedWords !== "")
                            ? msgText.height * 2 //we need 2x height in case of connected words
                            : msgText.height
            color: GamePlay.gamecourseModel.selectedMove === row
                   ? config.myPalette.highlight
                   : config.myPalette.window

            ShadowText {
                id: msgText
                width: parent.width
                height: fontMetrics.height
                color: parent.color
                shadowText: (model.column !== 3 || display > 0) ? display : "" //suppress 0 for not calculated (best) moves
                shadowColor: who > -1 && config.bColoredPlayers //color the words according the player, if checked
                             ? Qt.lighter(config.playercolors.get(who).itemColor)
                             : config.myPalette.windowText
                shadowPadding: (model.column === 1) ? 6 : 0 //some padding for the word column
                shadowVertAlign: Text.AlignTop
                shadowHorzAlign: (model.column === 1)
                                 ? config.ltr ? Text.AlignLeft : Text.AlignRight //word column aligns left
                                 : Text.AlignRight
            }
            Image {
                source: "qrc:///resources/bingo.png"
                visible: (model.column === 2) && isScrabble
                width: fontMetrics.height
                height: width
            }
            Text {
                id: msgConnected
                text: (model.column === 1) ? connectedWords : ""
                color: isDark(config.myPalette.window)
                       ? Qt.lighter(config.myPalette.windowText, 0.8)
                       : Qt.darker(config.myPalette.windowText, 0.8)
                leftPadding: (model.column === 1) ? 6 : 0 //some padding for the word column
                anchors.bottom: parent.bottom
                width: parent.width
                elide: Text.ElideRight
            }

            MouseArea {
                anchors.fill: parent
                //TODO: gamecourse: should be avail in network mode too
                onClicked: if (!GamePlay.isConnected) {
                    forceActiveFocus()
                    GamePlay.gamecourseModel.selectedMove = row
                }
            }
        }
    }
}
