import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    visible: GamePlay.isPoll
    title: qsTr("Poll")
    standardButtons: Dialog.Ok
    modal: true
    x: (app.width - about.width) / 2
    y: (app.height - about.height) / 2

    onAboutToShow: {
        var configData = {}
        configData = GamePlay.loadConfig("")
        var playerCount = configData["playerCount"] || 1
        player1.text = configData["player1"] || qsTr("1st Player");
        player2.text = configData["player2"] || qsTr("2nd Player");
        player2.enabled = playerCount > 1
        player3.text = configData["player3"] || qsTr("3rd Player");
        player3.enabled = playerCount > 2
        player4.text = configData["player4"] || qsTr("4th Player");
        player4.enabled = playerCount > 3
    }
    contentItem:
    GridLayout {
        columns: 2
        rowSpacing: 6
        columnSpacing: 12
        Label { id: player1 }
        Image { source: "qrc:///resources/" + GamePlay.answer[0] }
        Label { id: player2 }
        Image { source: "qrc:///resources/" + GamePlay.answer[1] }
        Label { id: player3 }
        Image { source: "qrc:///resources/" + GamePlay.answer[2] }
        Label { id: player4 }
        Image { source: "qrc:///resources/" + GamePlay.answer[3] }
        CheckBox {
            Layout.columnSpan: 2
            text: "Auto close when done"
            enabled: false
        }
    }
}
