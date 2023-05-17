import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtCharts

RowLayout {
    property int tbHeight: 22
    property string tip: "";

    Rectangle {
        id: statProgress
        Layout.preferredHeight: tbHeight
        Layout.preferredWidth: 100
        Layout.margins: 1
        Layout.fillWidth: mainLoader.state === "portrait" //messages are hidden in portrait mode
        border.color: config.myPalette.mid
        color: config.myPalette.window
        Rectangle {
            // using a real progressbar fails because background color does not
            // fit and applying a special background does not work on windows and macOS
            // without the item
            id: progressBar

            property int placedValue: GamePlay.placedValue
            property int computeProgress: GamePlay.computeProgress

            height: parent.height - 2
            y: 1
            color: computeProgress > 0 ? "lightblue" : "lightgreen"
            onPlacedValueChanged: {
                width = placedValue > 50 ? parent.width : Math.round(parent.width * placedValue/50)
                tip = qsTr("Score: %1 points").arg(placedValue)
            }
            onComputeProgressChanged: {
                width = Math.round(parent.width * computeProgress/100)
                tip = ""
                marquee.running = (computeProgress > 99)
            }
            gradient: Gradient {
                id: gradient
                property real pos: 0
                orientation: Gradient.Horizontal
                GradientStop { position: 0;            color: progressBar.color }
                GradientStop { position: gradient.pos; color: Qt.darker(progressBar.color) }
                GradientStop { position: 1;            color: progressBar.color }
            }
            SequentialAnimation {
                id: marquee
                loops: Animation.Infinite
                PropertyAnimation {
                    target: gradient
                    properties: "pos"
                    from: 0
                    to: 1
                    duration: 500
                }
                PropertyAnimation {
                    target: gradient
                    properties: "pos"
                    from: 1
                    to: 0
                    duration: 500
                }
                onRunningChanged: gradient.pos = 0
            }
        }
        MouseArea {
            id: maProgress
            anchors.fill: parent
            hoverEnabled: true
        }
        ToolTip {
            id: tipProgress
            text: tip
            visible: (tip.length>0) && maProgress.containsMouse
            delay: 10
            timeout: 5000
        }
    }
    Rectangle {
        id: statMessages
        visible: mainLoader.state === "landscape"
        Layout.preferredHeight: tbHeight
        Layout.fillWidth: true
        Layout.margins: 1
        border.color: config.myPalette.mid
        color: config.myPalette.window
        Text {
            leftPadding: 4
            width: parent.width
            height: parent.height
            color: config.myPalette.windowText
            text: GamePlay.lastError
            verticalAlignment: Text.AlignVCenter
            clip: true
            elide: Text.ElideRight
        }
    }
    Rectangle {
        id: statGameplay
        Layout.preferredHeight: tbHeight
        Layout.preferredWidth: statText.width
        Layout.margins: 1
        border.color: config.myPalette.mid
        color: config.myPalette.window
        Text {
            id: statText
            leftPadding: 4
            rightPadding: 4
            height: parent.height
            color: config.myPalette.windowText
            text: GamePlay.statInfo
            verticalAlignment: Text.AlignVCenter
        }
        ToolTip {
            id: statTip
            onVisibleChanged: text = GamePlay.allStat()
            visible: statMouseArea.containsMouse
            delay: 1000
            timeout: 5000
        }
        ActionGroup {
            id: statActionGroup
            exclusive: true
            Action {
                id: acLetters
                checkable: true
                checked: true
                text: qsTr("Letters left")
                onTriggered: GamePlay.statInfoType(0)
            }
            Action {
                id: acScore
                checkable: true
                text: qsTr("Game score")
                onTriggered: GamePlay.statInfoType(1)
            }
            Action {
                id: acTime
                checkable: true
                text: qsTr("Time left") //TODO: statusbar: "timeleft" with no limit
                onTriggered: GamePlay.statInfoType(2)
            }
        }
        Menu {
            id: statContextMenu
            MenuItem { action: acLetters; checked: true }
            MenuItem { action: acScore }
            MenuItem { action: acTime }
        }
        MouseArea {
            id: statMouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: (mouse) => {
                if (mouse.button === Qt.RightButton)
                    statContextMenu.popup()
            }
            onPressAndHold: statContextMenu.popup()
        }
    }

    Repeater {
        id: statPlayers
        model: GamePlay.numberOfPlayers
        Layout.margins: 1
        ShadowText {
            Layout.preferredHeight: tbHeight
            Layout.preferredWidth: shadowWidth + shadowPadding

            color: config.myPalette.window
            shadowColor: config.bColoredPlayers
                         ? Qt.lighter(config.playercolors.get(index).itemColor)
                         : "transparent"
            shadowText: GamePlay.getPlayerName(index);
            active: index === GamePlay.currentPlayer;
        }

    }
    Item {
        Layout.preferredHeight: tbHeight * poll.visible
        Layout.preferredWidth: tbHeight * poll.visible
        ChartView {
            id: poll
            visible: false
            anchors.fill: parent
            legend.visible: false
            antialiasing: true
            margins { top: 0; bottom: 0; left: 0; right: 0 }
            property int border: 1
            plotArea: Qt.rect(border, border, width - border*2, height - border*2)
            PieSeries { id: pieSeries; size: 1 }
        }
    }
    Connections {
        target: GamePlay
        function onShowPoll(doShow) {
            poll.visible = doShow
        }
        function onAnswerChanged(answers) {
            pieSeries.clear()
            for (var i=0; i<answers.length; i++) {
                var pie = pieSeries.append("", 1)
                switch (answers[i]) {
                case  0: pie.color = config.myPalette.dark; break
                case -1: pie.color = "red"; break
                case  1: pie.color = "green"; break
                }
                pie.borderWidth = 0
                pie.borderColor = pie.color
             }
        }
    }
}
