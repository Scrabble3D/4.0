import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

GridLayout {
    palette: config.myPalette
    columns: 2

    width: scrollView.width
    height: scrollView.height
    onWidthChanged: locTable.forceLayout()

    property int selectedLoc: -1 // -1 = nothing selected
    property int loadedLoc: -1

    HorizontalHeaderView {
        id: locHeader
        syncView: locTable
        model: GamePlay.locListModel
        Layout.leftMargin: 8
        Layout.columnSpan: 2
        boundsBehavior: Flickable.StopAtBounds

        delegate: Rectangle {
            implicitWidth: text.implicitWidth + 4
            implicitHeight: Math.max(locHeader.height,
                                     text.implicitHeight + 4)
            color: palette.mid
            border.color: palette.midlight
            Text {
                id: text
                text: model[locHeader.textRole]
                width: parent.width
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    TableView {
        id: locTable
        boundsBehavior: Flickable.StopAtBounds
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumHeight: 200
        Layout.leftMargin: 8
        Layout.columnSpan: 2
        ScrollBar.vertical: ScrollBar { id: sbLocTable}
        clip: true;
        columnWidthProvider: function(column) {
            return column < 2
                ? Math.floor((configUI.width - 12) * 1/3) //names
                : column < 4 //hide column with filename
                    ? Math.floor((configUI.width - 12) * 1/6) //versions
                    : 0
        }
        model: GamePlay.locListModel
        selectionModel: ItemSelectionModel {
            id: locSelect
            model: locTable.model
        }
        delegate: Rectangle {
            id: locDelegate
            required property bool selected
            implicitWidth: parent.width
            implicitHeight: delText.height + 2
            color: isLoaded ? palette.highlight
                            : selected ? Qt.lighter(palette.highlight)
                                       : palette.window
            border.color: palette.midlight

            Text {
                id: delText
                width: parent.width
                padding: 2
                color: isDark(parent.color) ? "white" : "black"
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                text: display
            }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onDoubleClicked: acLoadLocalization.trigger()
                onPressed: {
                    configUI.selectedLoc = model.row
                    locSelect.select(GamePlay.locListModel.index(model.row,0),
                                      ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows)
                }
                onClicked: (mouse)=> {
                    if (mouse.button === Qt.RightButton)
                        locContextMenu.popup()
                }
                onPressAndHold: locContextMenu.popup()
            }
        }
    }

    Menu {
        id: locContextMenu
        MenuItem { action: acLoadLocalization }
        MenuItem { action: acDeleteLocalization }
    }

    Action {
        id: acLoadLocalization
        enabled: loadedLoc != selectedLoc
        text: GamePlay.locListModel.data(GamePlay.locListModel.index(selectedLoc, 3)) === ""
                  ? qsTr("Download localization")
                  : qsTr("Load localization")
        onTriggered: {
            var fileName = GamePlay.locListModel.data(GamePlay.locListModel.index(selectedLoc, 4))
            GamePlay.localize(fileName)
//                loadedLoc = selectedLoc
//                loadedFileName = fileName
        }
    }
    Action {
        id: acDeleteLocalization
        text: qsTr("Delete local translation")
        enabled: GamePlay.locListModel.data(GamePlay.locListModel.index(selectedLoc,3)) !== ""
        onTriggered: {
            var fileName = GamePlay.locListModel.data(GamePlay.locListModel.index(selectedLoc, 4))
            if ( GamePlay.deleteLocFile(fileName) )
                GamePlay.addMessage(qsTr("Localization \"%1\" successfully deleted.").arg(fileName))
            else
                GamePlay.addMessage(qsTr("Localization \"%1\" failed to delete.").arg(fileName))
        }
    }
}
