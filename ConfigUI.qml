import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ListView {
    id: locView

    property int pad: 12
    width: rightPane.width - 2*pad
    height: rightPane.height - 2*pad
    leftMargin: pad
    topMargin: pad

    model: GamePlay.locListModel
    delegate: locViewItem
    boundsMovement: Flickable.StopAtBounds
    currentIndex: -1

    Menu {
        id: locContextMenu
        MenuItem { action: acLoadLocalization }
        MenuItem { action: acDeleteLocalization }
    }

    Action {
        id: acLoadLocalization
        enabled: !model.isLoaded
        text: model.get(currentIndex).installedversion !== ""
                  ? qsTr("Download localization")
                  : qsTr("Load localization")
        onTriggered: GamePlay.localize(model.get(currentIndex).filename)
    }
    Action {
        id: acDeleteLocalization
        text: qsTr("Delete local translation")
        enabled: model.get(currentIndex).installedVersion !== ""
        onTriggered: {
            var fileName = model.get(currentIndex).filename
            if ( GamePlay.deleteLocFile( fileName ) )
                GamePlay.addMessage(qsTr("Localization \"%1\" successfully deleted.").arg(fileName))
            else
                GamePlay.addMessage(qsTr("Localization \"%1\" failed to delete.").arg(fileName))
        }
    }

    TextMetrics {
        id: tm
        text: "ABC"
    }

    property var colWidth: [100,100,20,20];
    onWidthChanged: colWidth = [locView.width * 1/3,
                                locView.width * 1/3,
                                locView.width * 1/6,
                                locView.width * 1/6]
    header: Rectangle {
        id: listHeader
        width: locView.width
        height: tm.height + 8
        color: palette.mid
        Rectangle {
            id: entry1
            width: colWidth[0]; height: listHeader.height; color: "transparent"; border.color: palette.midlight
            ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Native") }
        }
        Rectangle {
            id: entry2
            anchors.left: entry1.right
            width: colWidth[1]; height: listHeader.height; color: "transparent"; border.color: palette.midlight
            ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter
                text: qsTr("English") }
        }
        Rectangle {
            id: entry3
            anchors.left: entry2.right
            width: colWidth[2]; height: listHeader.height; color: "transparent"; border.color: palette.midlight
            ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Installed") }
        }
        Rectangle {
            id: entry4
            anchors.left: entry3.right
            width: colWidth[3]; height: listHeader.height; color: "transparent"; border.color: palette.midlight
            ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Available") }
        }
    }

    Component {
        id: locViewItem
        Column {
            height: locViewDetails.visible
                    ? locViewEntry.height + locViewDetails.height
                    : locViewEntry.height

            Rectangle {
                id: locViewEntry
                width: locView.width
                height: tm.height + 4
                color: model.isLoaded
                       ? palette.highlight
                       : currentIndex == index
                         ? Qt.lighter(palette.highlight)
                         : palette.window
                Rectangle {
                    id: entry1
                    width: colWidth[0]; height: locViewEntry.height; color: "transparent"
                    ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter; bgcolor: locViewEntry.color;
                        text: model.native }
                }
                Rectangle {
                    id: entry2
                    anchors.left: entry1.right
                    width: colWidth[1]; height: locViewEntry.height; color: "transparent"
                    ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter; bgcolor: locViewEntry.color;
                        text: model.english }
                }
                Rectangle {
                    id: entry3
                    anchors.left: entry2.right
                    width: colWidth[2]; height: locViewEntry.height; color: "transparent"
                    ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter; bgcolor: locViewEntry.color;
                        text: model.installedversion }
                }
                Rectangle {
                    id: entry4
                    anchors.left: entry3.right
                    width: colWidth[3]; height: locViewEntry.height; color: "transparent"
                    ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter; bgcolor: locViewEntry.color;
                        text: model.availableversion }
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onDoubleClicked: if (!isLoaded)
                                        acLoadLocalization.trigger()
                    onPressed: locView.currentIndex = index
                    onClicked: (mouse)=> {
                                   if (mouse.button === Qt.RightButton)
                                   locContextMenu.popup()
                               }
                    onPressAndHold: locContextMenu.popup()
                } // MouseArea

            }
            GridLayout {
                id: locViewDetails
                visible: (locView.currentIndex === index) && (model.installedversion !== "")
                columns: 2
                rowSpacing: 1
                x: 16

                ColorLabel { Layout.topMargin: 12; Layout.bottomMargin: 12; text: qsTr("Author:") }
                ColorLabel { Layout.topMargin: 12; Layout.bottomMargin: 12; text: model.author }

            }
        }
    }
    RoundButton {
        id: actionButton
        implicitWidth: 50
        implicitHeight: 50
        x: rightPane.width - 75
        y: rightPane.height - 75
        enabled: !model.get(currentIndex).isLoaded
        display: AbstractButton.IconOnly
        icon.width: 32
        icon.height: 32
        icon.source: model.get(currentIndex).installedversion !== ""
                        ? "qrc:///resources/dictionary.png"
                        : "qrc:///resources/dictionarydown.png"
        background: Rectangle {
            anchors.fill: parent
            radius: width / 2
            gradient: Gradient {
                GradientStop {
                    position: actionButton.hovered ? 1.0 : 1.0
                    color: actionButton.enabled ? "darkgreen" : "lightgrey"
                }
                GradientStop {
                    position: actionButton.hovered ? 1.0 : 0.0
                    color: actionButton.enabled ? "limegreen" : "lightgrey"
                }
            }
        }
        ToolTip {
            text: qsTr("Load localization")
            visible: actionButton.hovered
            delay: 1000
            timeout: 5000
        }
        onPressed: acLoadLocalization.trigger()
    }

}
