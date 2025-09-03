import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl //iconimage
import QtQuick.Layouts
import Qt.labs.qmlmodels // delegate choice

Dialog {
    id: remotegame
    //: dialog title
    title: qsTr("Load game from server")
    modal: true
    width: 380 > screen.width ? screen.width : 380
    height: 360

    x: (scrabble3D.width - width) / 2
    y: (scrabble3D.height - height) / 2

    footer: DialogButtonBox {
        id: buttons
        standardButtons: Dialog.Ok | Dialog.Cancel
    }

    property int pad: 5 //padding for content

    property int selectedRow: 1
    onSelectedRowChanged: buttons.standardButton(Dialog.Ok).enabled = true

    onAboutToShow: {
        //disable Ok until something has been selected
        selectedRow = -1
        buttons.standardButton(Dialog.Ok).enabled = false
    }

    property var _roleName: ["lastaccess", "playernames", "moves"]
    property int _sortIndex: 0
    property int _sortOrder: 0

    contentItem: ColumnLayout {

        SortFilterProxyModel {
            id: sortModel
            model: GamePlay.remoteGames
            sorters: [
                RoleSorter {
                    roleName: _roleName[_sortIndex]
                    sortOrder: _sortOrder === 0 ? Qt.DescendingOrder : Qt.AscendingOrder
                }
            ]
        }
        HorizontalHeaderView {
            id: tableHeader
            syncView: tableView
            boundsMovement: Flickable.StopAtBounds
            Layout.fillWidth: true
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
            id: tableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            ScrollBar.vertical: ScrollBar {}
            clip: true
            boundsMovement: Flickable.StopAtBounds
            columnSpacing: 1
            rowSpacing: 3

            model: sortModel

            columnWidthProvider: function(column) {
                if (column < 2)
                    return (remotegame.implicitContentWidth - pad) * 0.4
                else
                    return (remotegame.implicitContentWidth - pad) * 0.2
            }

            TextMetrics { id: tm; text: "123" }

            delegate: Rectangle {
                color: selectedRow === row ? config.myPalette.highlight : "transparent"
                implicitHeight: 2 * tm.height
                implicitWidth: tableView.width
                Row {
                    spacing: 3
                    ColumnLayout {
                        spacing: 0
                        Layout.leftMargin: 3
                        Layout.rightMargin: 3
                        IconImage {
                            source: "qrc:///resources/create.png"
                            height: tm.height
                            width: height
                            visible: model.column === 0
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                            Layout.preferredHeight: tm.height
                            color: config.myPalette.buttonText
                        }

                        //TODO: QML remotegames: icon gameend
                        //TODO: QML remotegames: icon color own move
                        IconImage {
                            source: "qrc:///resources/change.png"
                            height: tm.height
                            width: height
                            visible: model.column === 0
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                            Layout.preferredHeight: tm.height
                            color: model.hasEnded
                                   ? "red"
                                   : model.isOwnTurn
                                     ? "green"
                                     : config.myPalette.buttonText
                        }
                    }
                    Text {
                        text: model.column === 0
                              ? model.created + "\n" + model.lastaccess
                              : model.column === 1
                                ? model.playernames
                                : model.column === 2
                                  ? model.moves
                                  : ""
                        color: selectedRow === row
                               ? config.myPalette.highlightedText // BUG: QML remotegames: highlightedtext color is black on dark blue on Android
                               : config.myPalette.windowText
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    TapHandler {
                        onTapped: selectedRow = row
                    }
                }
            }
        }
    }
    onAccepted: GamePlay.loadRemoteGame(selectedRow)

    /*
    sgRemote.RowCount:=aMsg.Value('NumberOfGames',1)+1;
    for i:=0 to aMsg.Value('NumberOfGames',0)-1 do
    begin
      sgRemote.Cells[6,i+1]:='0';         //active game, not on turn
      if aMsg.Value('Game'+inttostr(i)+'.GameEnd',false) then
        sgRemote.Cells[6,i+1]:='1' else   //ended game
      begin
        sl:=TStringList.Create;
        try
          sl.StrictDelimiter:=true;
          sl.Delimiter:=',';
          sl.DelimitedText:=aMsg.Value('Game'+inttostr(i)+'.Sequence','');
          if sl.Count>0 then
          begin
            j:=aMsg.Value('Game'+inttostr(i)+'.Moves',0) mod sl.Count;
            if (sl[j]=TCPClient.PlayerData.PlayerName) then
              sgRemote.Cells[6,i+1]:='2';  //active game, own turn
          end;
        finally
          sl.Free;
        end;
      end;
      sgRemote.Cells[1,i+1]:=DateTimeToStr(aMsg.Value('Game'+inttostr(i)+'.Date',Now));
      sgRemote.Cells[2,i+1]:=DateTimeToStr(aMsg.Value('Game'+inttostr(i)+'.LastAccess',Now));
      sgRemote.Cells[3,i+1]:=UTF8Encode(AnsiReplaceText(UTF8Decode(aMsg.Value('Game'+inttostr(i)+'.Player','')), '"', ''));
      sgRemote.Cells[4,i+1]:=aMsg.Value('Game'+inttostr(i)+'.Moves','');
      sgRemote.Cells[5,i+1]:=aMsg.Value('Game'+inttostr(i)+'.Name','');
*/
}
