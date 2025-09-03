import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    Layout.fillHeight: true
    Layout.fillWidth: true

    TextMetrics { id: tm }

    property var _roleName: ["move", "word", "value", "best", "time"]
    property int _sortIndex: 0
    property int _sortOrder: 0

    // TODO: SortFilterProxyModel needs to be on the Qt side
    // no key up/down when sorted possible, no true selection,
    // adding items when sorted not at the right position
    SortFilterProxyModel {
        id: sortModel
        model: GamePlay.gamecourseModel
        sorters: [
            RoleSorter {
                roleName: _roleName[_sortIndex]
                sortOrder: _sortOrder === 0 ? Qt.DescendingOrder : Qt.AscendingOrder
            }
        ]
    }

    HorizontalHeaderView {
        id: tableHeader
        Layout.fillWidth: true
        syncView: statTable
        boundsMovement: Flickable.StopAtBounds
        delegate: HorizontalHeaderViewDelegate {
            TapHandler {
                onTapped: {
                    if (index == _sortIndex)
                        _sortOrder ^= 1
                    else
                        _sortIndex = index
                }
            }
        }
    }

    TableView {
        id: statTable
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true
        boundsMovement: Flickable.StopAtBounds

        columnWidthProvider: function(column) {
            const pad = 12
            var col = []
            for (var i = 0; i < 5; i++)
            {
                tm.text = model.headerData(i, Qt.Horizontal)
                col[i] = tm.width + 2 * pad
            }
            col[1] = statTable.width - pad - col[0] - col[2] - col[3] - col[4]
            return col[column]
        }
        ScrollBar.vertical: ScrollBar { }
        model: sortModel
        columnSpacing: 1
        rowSpacing: 1
/*
        Keys.onPressed: (event)=> {
            if (event.key === Qt.Key_Down)
                GamePlay.gamecourseModel.selectedMove++;
            else if (event.key === Qt.Key_Up)
                GamePlay.gamecourseModel.selectedMove--;
            positionViewAtRow(GamePlay.gamecourseModel.selectedMove, TableView.Contain | TableView.AlignVCenter)
            if (GamePlay.is3D)
                cube.roll()
        }
*/
        delegate: Rectangle {
            implicitWidth: parent.width
            implicitHeight: (connectedWords !== "")
                            ? tm.height * 2 //we need 2x height in case of connected words
                            : tm.height
            color: GamePlay.gamecourseModel.selectedMove === model.move
                   ? config.myPalette.highlight
                   : config.myPalette.window
            ShadowText {
                id: msgText
                width: parent.width
                height: tm.height
                color: parent.color
                shadowText: model.column === 0
                            ? move
                            : model.column === 1
                              ? word
                              : model.column === 2
                                ? value
                                : model.column === 3
                                  ? best
                                  : model.column === 4
                                    ? time
                                    : ""
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
                width: tm.height
                height: width
            }
            Text {
                id: msgConnected
                text: (model.column === 1) ? connectedWords :
                      (model.column === 2) ? bonuswho > -1 ? bonus > 0 ? "+" + bonus : bonus
                                                           : ""
                                           : ""
                style: (model.column === 2) && bonuswho > -1 ? Text.Outline : Text.Normal
                styleColor: (model.column === 2) && bonuswho > -1 ? Qt.lighter(config.playercolors.get(bonuswho).itemColor) : ""
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
                //TODO: gamecourse: history should be avail in network mode too
                onClicked: if (!GamePlay.isConnected) {
                    forceActiveFocus()
                    GamePlay.gamecourseModel.selectedMove = model.move
                }
            }
        }
    }
}
