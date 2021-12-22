import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Universal

ColumnLayout {
    Layout.fillHeight: true
    Layout.fillWidth: true

    property var colwidths: [20,0,0,0,0] //use 20px for first column and calculate 0 in respect to parent

    SplitView {
        id: statHeader
        Layout.fillWidth: true
        implicitHeight: 25 //TODO: fix value
        Repeater {
            id: headerColumn
            model: [qsTr("#"),qsTr("Word"),qsTr("Value"),qsTr("Best"),qsTr("Time")]
            Item {
                SplitView.minimumWidth: text.implicitWidth
                SplitView.preferredWidth: colwidths[index] == 0 ? parent.width / 5 - 20 : colwidths[index]
                SplitView.fillWidth: index == 1 ? true : false //expand the "Word" column
                implicitHeight: text.implicitHeight + 2
                clip: true

                Rectangle {
                    anchors.fill: parent
                    anchors.leftMargin: -border.width
                    anchors.topMargin:  -border.width
                    anchors.bottomMargin: -border.width
                    border.width: 1
                    border.color: Universal.baseMediumLowColor
                    color: Universal.chromeHighColor //to "hide" the large splitter handles
//TODO: palette. instead of Universal
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
                        horizontalAlignment: index == 1 ? Text.AlignLeft : Text.AlignRight
                        leftPadding: 5
                        rightPadding: 5
                        color: Universal.foreground
                    }
                }
            }
        }

    }

    TableView { //TODO: sort
        id: statTable
        Layout.fillHeight: true
        Layout.fillWidth: true
        clip: true
        columnWidthProvider: function(column) { return colwidths[column] }
        ScrollBar.vertical: ScrollBar { }
        model: GamePlay.statModel
        columnSpacing: 1
        rowSpacing: 1
        delegate: Rectangle {
            implicitWidth: parent.width
            implicitHeight: connectedWords !== "" ? msgText.height * 2 : msgText.height

            ShadowText {
                id: msgText
                width: parent.width
                height: 20 //TODO: should depend on font size
                shadowText: display
                shadowColor: who > -1 && config.colorplayers
                       ? Qt.lighter(config.playercolors.get(who).itemColor)
                       : "transparent"
                shadowPadding: column == 1 ? 6 : 0
                shadowVertAlign: Text.AlignTop
                shadowHorzAlign: column !== 1 ? Text.AlignRight : Text.AlignLeft
            }
            Text {
                id: msgConnected
                text: column == 1 ? connectedWords : ""
                color: "darkgrey"
                leftPadding: column == 1 ? 6 : 0
                anchors.bottom: parent.bottom
                width: parent.width
                elide: Text.ElideRight
            }
        }
    }

}
