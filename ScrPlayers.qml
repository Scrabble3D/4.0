import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: layout

    Layout.fillHeight: true
    Layout.fillWidth: true

    property alias treeview: treeview

    Action {
        id: acInvite
        text: qsTr("Invite")
        onTriggered: GamePlay.doInvite(treeview.selected)
    }
    Action {
        id: acLeave
        text: qsTr("Leave group")
        onTriggered: GamePlay.doLeave()
    }
    Action {
        id: acOpenForGames
        text: qsTr("Open for games")
        checkable: true
        checked: true
        onTriggered: GamePlay.setOpenForGames(checked)
    }

    Menu {
        id: contextMenu
        MenuItem { action: acInvite }
        MenuItem { action: acLeave }
        MenuItem {
            id: miOpenForGames
            action: acOpenForGames
            visible: false
            height: visible ? implicitHeight : 0
        }
    }

    TreeView {
        id: treeview

        Layout.fillHeight: true
        Layout.fillWidth: true

        visible: true

        property string selected: ""

        ScrollBar.vertical: ScrollBar { }

        model: GamePlay.playersTreeModel

        delegate: Rectangle {
            id: treeDelegate

            color: (treeView.selected === model.name)
                       ? config.myPalette.highlight
                       : "transparent"

            implicitWidth: layout.parent.width //3*padding + label.x + label.implicitWidth
            implicitHeight: label.implicitHeight * 1.2

            readonly property real indent: 20
            readonly property real padding: 5

            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property int hasChildren
            required property int depth

            //TODO: QML players: remember expanded state
            onTreeViewChanged: treeView.toggleExpanded(row)

            function getText(md) { //suppress Unable to assign [undefined] to QString
                if (md) return md
                   else return ""
            }

            ToolTip {
                id: toolTip
                visible: false
                delay: 1000
                timeout: 5000

                background: Rectangle {
                    border.width: 0
                    color: "#f6f6b9" // ensure black font color on light yellow background
                }

                property var names: [qsTr("Country:"), qsTr("City:"), qsTr("Language:"),
                                     qsTr("Rating:"), qsTr("#Games:"), qsTr("Registered:"),
                                     qsTr("Release:")]
                property var values: []
                onAboutToShow: {
                    values = [ getText(model.country), getText(model.city), getText(model.menuLang),
                               getText(model.rating), getText(model.gamesPlayed), getText(model.registered),
                               getText(model.release) ]
                }
                contentItem: GridLayout {
                    columns: 2
                    rowSpacing: 1
                    Repeater {
                        model: toolTip.names
                        Label {
                            Layout.row: index
                            Layout.column: 0
                            text: modelData
                            color: "black"
                            Layout.alignment: Qt.AlignRight
                        }
                    }
                    Repeater {
                        model: toolTip.values
                        Label {
                            Layout.row: index
                            Layout.column: 1
                            text: modelData
                            color: "black"
                            Layout.alignment: Qt.AlignLeft
                        }

                    }
                } //GridLayout
            }
            HoverHandler {
                id: hoverHandler
                onHoveredChanged: toolTip.visible = hovered && !treeDelegate.hasChildren
            }
            TapHandler {
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onTapped: (eventPoint, button)=> {
                    if (treeDelegate.hasChildren)
                        treeView.toggleExpanded(row)
                    else {
                        treeView.selected = model.name
                        if (button ===  Qt.RightButton)
                            longPressed() //contextMenu.popup()
                    }
                }
                onLongPressed:
                    if (treeView.selected === model.name) //long press on selected item
                    {
                        acLeave.enabled = (depth === 1) && (treeView.selected === GamePlay.networkName()) //in a group
                        acInvite.enabled = (treeView.selected !== GamePlay.networkName()) &&  //don't invite yourself
                                           (model.version === 4) && // don't group with old program version
                                           (model.isOpenForGames) // open for games
                        miOpenForGames.visible = treeView.selected === GamePlay.networkName()
                        contextMenu.popup()
                    }
            }

            Text {
                id: indicator
                visible: treeDelegate.isTreeNode && treeDelegate.hasChildren
                x: padding + (treeDelegate.depth * treeDelegate.indent)
                anchors.verticalCenter: playerImage.verticalCenter
                text: "▸"
                rotation: treeDelegate.expanded ? 90 : 0
            }
            Image {
                id: playerImage
                x: indicator.x + indicator.width + padding
                source: treeDelegate.hasChildren
                        ? "qrc:///resources/group.png"
                        : "qrc:///resources/player.png"
            }
            Image {
                id: flagImage
                source: (model.flag !== "unknown") && (model.flag !== "undefined")
                        ? "qrc:///flags/" + model.flag
                        : ""
                x: playerImage.x + 9
                y: playerImage.y + 13
                width: 9
                height: 6
            }
            Image {
                id: playerState
                source: (model.isOpenForGames || treeDelegate.hasChildren)
                        ? ""
                        : "qrc:///resources/notopen.png"
                x: playerImage.x + 13
                y: playerImage.y + 1
                width: 7
                height: 7
            }

            Text {
                id: label
                x: playerImage.x + playerImage.width + padding
                anchors.verticalCenter: playerImage.verticalCenter
                clip: true
                color: config.myPalette.windowText
                text: model.name
            }
        }
    }
}
