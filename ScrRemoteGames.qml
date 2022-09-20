import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels // delegate choice

Dialog {
    id: remotegame
    title: qsTr("Start a new game")
    modal: true
    padding: mainLoader.state === "landscape" ? 12 : 0
    x: (scrabble3D.width - remotegame.width) / 2
    y: (scrabble3D.height - remotegame.height) / 2

    footer: DialogButtonBox {
        id: buttons
        standardButtons: Dialog.Ok | Dialog.Cancel
    }

    property int pad: 5 //padding for content
    property var colWidths: [120,150,50] //default values for column width
    onColWidthsChanged: contentItemChanged() //update column width to header/content

    property int selectedRow: 1
    onSelectedRowChanged: buttons.standardButton(Dialog.Ok).enabled = true

    TextMetrics {
        id: textMetrics
    }
    function getWidth(text) {
        textMetrics.text = text
        return textMetrics.width
    }

    onAboutToShow: {
        //disable Ok until something has been selected
        selectedRow = -1
        buttons.standardButton(Dialog.Ok).enabled = false

        //assigning value at initialization produces a binding loop warning
        colWidths[0] = Math.max( getWidth(headerText.get(0).text) + 2*pad,
                                 getWidth("01.01.1970 00:00") + pad + textMetrics.height)
        colWidths[1] = Math.max( getWidth(headerText.get(1).text) + 2*pad,
                                 150)
        colWidths[2] = Math.max( getWidth(headerText.get(2).text) + 2*pad,
                                 getWidth("123"))
    }

    ListModel {
        id: headerText
        ListElement { text: qsTr("Date/Time") }
        ListElement { text: qsTr("Players") }
        ListElement { text: qsTr("Moves") }
    }

    contentItem: Column {
        HorizontalHeaderView {
            id: tableHeader
            syncView: tableView
            model: headerText
            textRole: "text"
            //TODO: remote games: sort on click
//            TapHandler { onTapped: console.log(model.) }
        }
        TableView {
            id: tableView
            ScrollBar.vertical: ScrollBar {}
            implicitHeight: 300
            implicitWidth: colWidths[0] + colWidths[1] + colWidths[2]
            model: GamePlay.remoteGames

            columnWidthProvider: function(column) {
                return colWidths[column]
            }

            DelegateChooser {
                id: chooser
                DelegateChoice {
                    column: 0
                    Rectangle {
                        id: bgRect
                        color: selectedRow === row
                               ? config.myPalette.highlight
                               : "transparent"
                        implicitHeight: textMetrics.height * 2 + pad * 2
                        Column {
                            Row {
                                spacing: pad
                                leftPadding: pad
                                topPadding: pad
                                IconImage {
                                    id: imgDate
                                    anchors.verticalCenter: contentDate.verticalCenter
                                    source: "qrc:///resources/create.png"
                                    color: config.myPalette.buttonText
                                    height: 12//textMetrics.height
                                    width: height
                                }
                                Text {
                                    id: contentDate
                                    text: model.created
                                    color: selectedRow === row
                                           ? config.myPalette.highlightedText
                                           : config.myPalette.windowText
                                }
                            }
                            Row {
                                spacing: pad
                                leftPadding: pad
                                bottomPadding: pad
                                //TODO: remotegames icon gameend
                                //FIXME: remotegames icon color own move
                                IconImage {
                                    id: imgModified
                                    source: "qrc:///resources/change.png"
                                    anchors.verticalCenter: contentText.verticalCenter
                                    height: 12//textMetrics.height
                                    width: height
                                    color: model.hasEnded
                                           ? "red"
                                           : model.isOwnTurn
                                             ? "green"
                                             : config.myPalette.buttonText
                                }
                                Text {
                                    id: contentText
                                    text: model.lastaccess
                                    color: selectedRow === row
                                           ? config.myPalette.highlightedText
                                           : config.myPalette.windowText
                                }
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
                DelegateChoice {
                    column: 1
                    Rectangle {
                        color: selectedRow === row ? config.myPalette.highlight : "transparent"
                        implicitHeight: plNames.height
                        Text {
                            id: plNames
                            text: model.playernames
                            verticalAlignment: Qt.AlignTop
                            color: config.myPalette.windowText
                            width: colWidths[1]
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            padding: pad
                        }
                        MouseArea {
                            anchors.fill: parent
                            TapHandler {
                                onTapped: selectedRow = row
                            }
                        }
                    }
                }
                DelegateChoice {
                    column: 2
                    Rectangle {
                        color: selectedRow === row ? config.myPalette.highlight : "transparent"
                        Text {
                            text: model.moves
                            width: colWidths[2] //textMetrics.width
                            verticalAlignment: Qt.AlignTop
                            horizontalAlignment: Qt.AlignRight
                            color: config.myPalette.windowText
                            padding: pad
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

            delegate: chooser
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
