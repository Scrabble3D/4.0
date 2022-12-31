import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
//TODO: dictionary rework from ground; QTreeView or QConcatenateTablesProxyModel
GridLayout {
    columns: 2
    //TODO: config: responsive width
    // Layout.fillWidth: true
    // Layout.fillHeight: true
    width: scrollView.width
    height: scrollView.height
    onWidthChanged: dictTable.forceLayout()

    property alias categoriesRepeater: categoriesRepeater

    function getDictionaryName() {
        var sResult = ""
        if (configDictionary.loadedDic > -1) {
            sResult = GamePlay.dicListModel.data(GamePlay.dicListModel.index(configDictionary.loadedDic,0))
            if (sResult === "") //local dictionary have no "NativeName" role and take the file name
                sResult = GamePlay.dicListModel.data(GamePlay.dicListModel.index(configDictionary.loadedDic,2))
        }
        return sResult
    }

    property int selectedDic: -1 // -1 = nothing selected
    property int loadedDic: -1

    //called from ScrConfig when loading a configuration
    function loadFromName(fileName) {
        var m = GamePlay.dicListModel
        for (var i=0; i<m.rowCount(); i++)
            if (m.data(m.index(i,2)) === fileName) {
                configDictionary.selectedDic = i
                dictSelect.select(m.index(i,0), ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows)
                acLoadDictionary.trigger()
                return true;
        }
        return false;
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

    function updateLetterDistribution() {
        var letterlist = [];
        letterlist = GamePlay.getLetterDistribution( config.getLetterSet(-1) )
        if (letterlist.length > 0)
            config.setLetterSet(letterlist)
    }

    Action {
        id: acLoadDictionary
        enabled: loadedDic != selectedDic
        text: GamePlay.dicListModel.data(GamePlay.dicListModel.index(selectedDic,4)) === ""
                  ? qsTr("Download dictionary")
                  : qsTr("Load dictionary")
        onTriggered: {
            var fileName = GamePlay.dicListModel.data(GamePlay.dicListModel.index(selectedDic,2))
            if (GamePlay.loadDictionary(fileName)) {
                GamePlay.addMessage(qsTr("Dictionary \"%1\" successfully loaded.").arg(fileName))
                //FIXME: configdic/dictionary: downloading dic is finsihed, leading to update before subsequent loading -> distribution not applied
                updateInfo() //categries
                updateLetterDistribution();
                loadedDic = selectedDic
            }
        }
    }
    Action {
        id: acDeleteDictionary
        text: qsTr("Delete local dictionary")
        enabled: GamePlay.dicListModel.data(GamePlay.dicListModel.index(selectedDic,4)) !== ""
        onTriggered: {
            var fileName = GamePlay.dicListModel.data(GamePlay.dicListModel.index(selectedDic,2))
            if (GamePlay.deleteDictionary(fileName)) {
                GamePlay.addMessage(qsTr("Dictionary \"%1\" successfully deleted.").arg(fileName))
                if (loadedDic == selectedDic) loadedDic = -1;
                updateInfo() //categries
            } else
                GamePlay.addMessage(qsTr("Deletion of \"%1\" failed.").arg(fileName))
        }
    }

    HorizontalHeaderView {
        id: dictHeader
        syncView: dictTable
        model: GamePlay.dicListModel
        Layout.leftMargin: 8
        Layout.columnSpan: 2
        boundsBehavior: Flickable.StopAtBounds

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

    Menu {
        id: dictContextMenu
        MenuItem { action: acLoadDictionary }
        MenuItem { action: acDeleteDictionary }
    }

    TableView {
        id: dictTable
        boundsBehavior: Flickable.StopAtBounds
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumHeight: 200 //in case of long comments or many categories
        Layout.leftMargin: 8
        Layout.columnSpan: 2
        ScrollBar.vertical: ScrollBar { id: sbDictTable}
        clip: true;
        columnWidthProvider: function(column) {
            return column < 2
                ? Math.floor((configDictionary.width - 12) * 1/3) //names
                : column > 2 //hide column with filename
                    ? Math.floor((configDictionary.width - 12) * 1/6) //versions
                    : 0
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
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onDoubleClicked: acLoadDictionary.trigger()
                onPressed: {
                    configDictionary.selectedDic = model.row
                    dictSelect.select(GamePlay.dicListModel.index(model.row,0),
                                      ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows)
                }
                onClicked: (mouse)=> {
                    if (mouse.button === Qt.RightButton)
                        dictContextMenu.popup()
                }
                onPressAndHold: dictContextMenu.popup()
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
        Layout.rightMargin: 50
        enabled: loadedDic != selectedDic
        display: AbstractButton.IconOnly
        icon.width: 32
        icon.height: 32
        icon.source: GamePlay.dicListModel.data(
                         GamePlay.dicListModel.index(configDictionary.selectedDic,4)) !== ""
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
