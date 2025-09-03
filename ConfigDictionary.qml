import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ListView {
    id: dictView

    width: rightPane.width
    height: rightPane.height - tm.height

    model: GamePlay.dicListModel
    delegate: dictViewItem
    boundsMovement: Flickable.StopAtBounds
    currentIndex: -1

    ScrollBar.vertical: ScrollBar { policy: Qt.ScrollBarAlwaysOn }

    property string dictionaryName: "" //used via ScrConfig in ScrNewGame
    property string dictionaryFile: "" //used in ScrConfig to load/save config
    property string dictionaryCategories: "" //used in ScrConfig to load/save config; must not be a string!
    property string letterDistribution: "" //submitted to configLetter setLetterSet() in msgDlg()

    function setCategories(aCategories) {
        /**
          + in network mode the dictionary could be the same
             but categories selected differently; this function just sets the
             categories and resets the model per setCategoryChecked(*,*,true)
             to update on the receiver
          + called from ScrConfig > applyConfig
          + not necessary in local mode since loading the dictionary also applies
            the categories
        **/
        for (var i = 0; i < dictionaryCategories.length; i++) {
            var isChecked = aCategories.indexOf(dictionaryCategories[i]) > -1
            GamePlay.setCategoryChecked(dictionaryCategories[i], isChecked, true)
        }
    }

    Connections {
        target: GamePlay

        function onLoadingFinished(aId) {
            dictView.currentItemChanged()
            var dicInfo = model.get(aId)
            dictionaryFile = dicInfo.filename
            dictionaryName = dicInfo.english +
                             " (" + dicInfo.native  + ")"
            dictionaryCategories = dicInfo.categories
            letterDistribution = dicInfo.distribution
            const isEqual = (letterDistribution.localeCompare(config.getLetterSet(-1)) === 0)

            if ( (!isEqual) &&  // do not ask when equal
                 (config.visible === true) && // do not ask on initial loading
                 (letterDistribution !== "") )  // do not ask if nothing is stored
                messageDialog.open()
        }
    }
    MessageDialog {
        id: messageDialog
        title: qsTr("Letter Distribution")
        text: qsTr("Letter distribution in dictionary does not match the current configuration.");
        informativeText: qsTr("Do you want to update the letter set?");
        buttons: MessageDialog.Yes | MessageDialog.No
        onAccepted: configLetter.setLetterSet( letterDistribution.split(",") )
    }

    Menu {
        id: dictContextMenu
        MenuItem { action: acLoadDictionary }
        MenuItem { action: acDeleteDictionary }
    }

    Action {
        id: acLoadDictionary
        enabled: !model.isLoaded
        text: model.get(currentIndex).installedversion !== ""
              ? qsTr("Load dictionary")
              : qsTr("Download dictionary")
        onTriggered: GamePlay.loadDictionary( model.get(currentIndex).filename, "" )
    }
    Action {
        id: acDeleteDictionary
        text: qsTr("Delete local dictionary")
        enabled: model.get(currentIndex).installedVersion !== ""
        onTriggered: GamePlay.deleteDictionary( model.get(currentIndex).filename )
    }

    TextMetrics {
        id: tm
        text: "ABC"
    }

    property var colWidth: [100,100,20,20];
    onWidthChanged: colWidth = [dictView.width * 1/3,
                                dictView.width * 1/3,
                                dictView.width * 1/6,
                                dictView.width * 1/6]

    header: Rectangle {
        id: listHeader
        width: dictView.width
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
        id: dictViewItem
        Column {
            height: dictViewDetails.visible
                    ? dictViewEntry.height + dictViewDetails.height
                    : dictViewEntry.height
            Rectangle {
                id: dictViewEntry
                width: dictView.width
                height: tm.height + 4
                color: model.isLoaded
                       ? palette.highlight
                       : currentIndex === index
                         ? Qt.lighter(palette.highlight)
                         : palette.window
                property var fgCol: isDark(dictViewEntry.color) ? "white" : "black"
                Rectangle {
                    id: entry1
                    width: colWidth[0]; height: dictViewEntry.height; color: "transparent"
                    ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter; bgcolor: dictViewEntry.color
                        text: model.native }
                }
                Rectangle {
                    id: entry2
                    anchors.left: entry1.right
                    width: colWidth[1]; height: dictViewEntry.height; color: "transparent"
                    ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter; bgcolor: dictViewEntry.color
                        text: model.english }
                }
                Rectangle {
                    id: entry3
                    anchors.left: entry2.right
                    width: colWidth[2]; height: dictViewEntry.height; color: "transparent"
                    ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter; bgcolor: dictViewEntry.color
                        text: model.installedversion }
                }
                Rectangle {
                    id: entry4
                    anchors.left: entry3.right
                    width: colWidth[3]; height: dictViewEntry.height; color: "transparent"
                    ColorLabel { x:2; width: parent.width-4; clip: true; anchors.verticalCenter: parent.verticalCenter; bgcolor: dictViewEntry.color
                        text: model.availableversion }
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onDoubleClicked: if (!isLoaded)
                                        acLoadDictionary.trigger()
                    onPressed: dictView.currentIndex = index
                    onClicked: (mouse)=> {
                                    if (mouse.button === Qt.RightButton)
                                        dictContextMenu.popup()
                               }
                    onPressAndHold: dictContextMenu.popup()
                } // MouseArea
            }
            GridLayout {
                id: dictViewDetails
                width: dictView.width
                visible: (dictView.currentIndex === index) && (model.installedversion !== "")

                columns: 2
                rowSpacing: 1
                x: 16

                ColorLabel {
                    Layout.topMargin: 12
                    Layout.alignment: Qt.AlignTop
                    text: qsTr("Author:")
                }
                ColorLabel {
                    Layout.topMargin: 12
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: model.author
                }

                ColorLabel {
                    Layout.alignment: Qt.AlignTop
                    text: qsTr("License:")
                }
                ColorLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: model.license
                }

                ColorLabel { text: qsTr("Release:"); }
                ColorLabel { text: model.release }

                ColorLabel {
                    Layout.alignment: Qt.AlignTop
                    text: qsTr("Comment:")
                }
                ColorLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: model.comment
                }

                ColorLabel {
                    Layout.alignment: Qt.AlignTop
                    Layout.topMargin: 2 // not aligned with first checkbox from repeater
                    Layout.minimumHeight: tm.height
                    text: qsTr("Categories:")
                }
                Column {
                    id: catColumns
                    Layout.minimumHeight: tm.height
                    Layout.bottomMargin: 12
                    property var catModel: model.categories
                    property bool isLoaded: model.isLoaded
                    Repeater {
                        id: catRepeater
                        model: parent.isLoaded ? parent.catModel : "" // don't show if not loaded (but keep bottom margin)
                        delegate: ColorCheckBox {
                            padding: 2
                            enabled: (index > 0) //first standard category is the default
                            checked: GamePlay.getCategoryChecked(modelData)
                            onCheckedChanged: GamePlay.setCategoryChecked(modelData, checked)
                            text: modelData
                        }
                    } //Repeater
                } //Column
            }
        }
    }
    onCurrentIndexChanged: {
        actionButton.isActive =
            !model.get(currentIndex).isLoaded
        actionButton.tiptext =
            model.get(currentIndex).installedversion !== ""
                ? qsTr("Load dictionary")
                : qsTr("Download dictionary")
        actionButton.icon.source =
            model.get(currentIndex).installedversion !== ""
                ? "qrc:///resources/dictionary.png"
                : "qrc:///resources/dictionarydown.png"
        actionButton.color =
            !model.get(currentIndex).isLoaded
                ? model.get(currentIndex).installedversion ? "limegreen" : "steelblue"
                : "darkgrey"
        actionButton.progress = 0 // repaint the background
    }
    ProgressButton {
        id: actionButton
        x: rightPane.width - 75
        y: rightPane.height - 75
        property int computeProgress: GamePlay.computeProgress //gameplay.cpp: SIGNAL(onProgress(int)) => SLOT(setComputeProgress(int))
        onComputeProgressChanged: progress = computeProgress

        MouseArea {
            anchors.fill: parent
            onClicked: if (actionButton.isActive) acLoadDictionary.trigger()
        }
    }

}
