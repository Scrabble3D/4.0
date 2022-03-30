import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//TODO: dictionary rework from ground; QTreeView or QConcatenateTablesProxyModel
GridLayout {
    id: configDictionary
    columns: 2

    width: scrollView.width
    height: scrollView.height
    onWidthChanged: dictTable.forceLayout()

    property alias acLoadDictionary: acLoadDictionary
    property alias categoriesRepeater: categoriesRepeater

    property string dictionaryName: GamePlay.dicListModel.data(GamePlay.dicListModel.index(configDictionary.selectedDic,0))

    property int selectedDic: -1 // -1 = nothing selected
    property int loadedDic: -1

    function loadFromName(fileName) {
        var m = GamePlay.dicListModel
        for (var i=0; i<m.rowCount(); i++)
            if (m.data(m.index(i,2)) === fileName) {
                configDictionary.selectedDic = i
                dictSelect.select(m.index(i,0), ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows)
                acLoadDictionary.trigger()
                break;
        }
    }

    function updateInfo() {
        var aData = GamePlay.selectedDicInfo(selectedDic)
        lbAuthor.text = aData["author"]
        lbLicense.text = aData["license"]
        lbComment.text = aData["comment"]
        lbRelease.text = aData["release"]
        //categories
        var cList = aData["categories"].split(",")
        var obj = {}
        categoriesModel.clear()
        for (var i=0; i<cList.length; i++)
            if (cList[i] !== "") {
                obj["name"] =cList[i]
                categoriesModel.append(obj)
            }
    }

    Action {
        id: acLoadDictionary
        onTriggered: {
            var fileName = GamePlay.dicListModel.data(GamePlay.dicListModel.index(configDictionary.selectedDic,2))
            var dicName = GamePlay.dicListModel.data(GamePlay.dicListModel.index(configDictionary.selectedDic,0))
            if (GamePlay.loadDictionary(fileName)) {
                GamePlay.addMessage(qsTr("Dictionary %1 successfully loaded.").arg("dicName"))
                updateInfo() //categries
                loadedDic = selectedDic
            }
        }
    }

    HorizontalHeaderView {
        id: dictHeader
        syncView: dictTable
        model: GamePlay.dicListModel
        Layout.leftMargin: 8
        Layout.columnSpan: 2

        delegate: Rectangle {
            implicitWidth: text.implicitWidth + 4
            implicitHeight: Math.max(dictHeader.height,
                                     text.implicitHeight + 4)
            color: myPalette.mid
            border.color: myPalette.midlight
            Text {
                id: text
                text: model[dictHeader.textRole]
                width: parent.width
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

    }

    TableView {
        id: dictTable
        Layout.fillHeight: true
        Layout.preferredWidth: scrollView.width + sbDictTable.width
        Layout.minimumHeight: 200 //in case of long comments or many categories
        Layout.leftMargin: 8
        Layout.columnSpan: 2
        ScrollBar.vertical: ScrollBar { id: sbDictTable}
        clip: true;
        columnWidthProvider: function(column) {
            return column < 2
                    ? configDictionary.width * 1/4
                    : configDictionary.width * 1/8
        }
        model: GamePlay.dicListModel
        selectionModel: ItemSelectionModel {
            id: dictSelect
            model: dictTable.model
            onSelectionChanged: updateInfo()
        }
        delegate: Rectangle {
            id: dictDelegate
            required property bool selected
            implicitWidth: parent.width
            implicitHeight: delText.height + 2
            color: isLoaded ? myPalette.highlight
                              : selected ? Qt.lighter(myPalette.highlight)
                                         : myPalette.light
            border.color: myPalette.midlight

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
                onDoubleClicked:
                    acLoadDictionary.trigger()
                onClicked: {
                    configDictionary.selectedDic = model.row
                    dictSelect.select(GamePlay.dicListModel.index(model.row,0),ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows)
                }
            }
        }
    }

    RoundButton {
        id: actionButton
        implicitWidth: 50
        implicitHeight: 50
        Layout.column: 1 //below dictTable
        Layout.row: 2
        Layout.alignment: Qt.AlignRight
        Layout.topMargin: -25 //overlapping with dictTable
        Layout.rightMargin: 25
        enabled: loadedDic != selectedDic
        display: AbstractButton.IconOnly
        icon.source: "qrc:///resources/dictionary.png"
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
            text: qsTr("Load dictionary")
            visible: actionButton.hovered
            delay: 1000
            timeout: 5000
        }
        onPressed: acLoadDictionary.trigger()
    }

    Label { Layout.leftMargin: 16; text: qsTr("Author:"); }
    Label { id: lbAuthor }
    Label { Layout.leftMargin: 16; text: qsTr("License:"); }
    Label { id: lbLicense }
    Label { Layout.leftMargin: 16; text: qsTr("Release:"); }
    Label { id: lbRelease }
    Label {
        Layout.leftMargin: 16
        Layout.bottomMargin: 32
        Layout.alignment: Qt.AlignTop
        text: qsTr("Comment:")
    }
    Label {
        Layout.bottomMargin: 32
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        id: lbComment
    }
    ListModel {
        id: categoriesModel
    }
    Label {
        Layout.alignment: Qt.AlignTop
        Layout.leftMargin: 16
        text: qsTr("Categories:")
    }
    ColumnLayout {
        spacing: 0
        Repeater {
            id: categoriesRepeater
            model: categoriesModel
            delegate: CheckBox {
                enabled: index>0 //first category is the default
                checked: GamePlay.getCategory(name)
                //NOTE: configdictionary: categories should be static during the game but that implies to also keep the dictionary loaded
                onCheckedChanged: GamePlay.setCategory(name, checked)
                text: name
            }
        }
    }

}
