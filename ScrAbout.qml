import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: about
    title: qsTr("About Scrabble3D")
    standardButtons: Dialog.Close
    modal: true
    clip: true
    width: 380 > screen.width ? screen.width : 380

    x: (scrabble3D.width - about.width) / 2
    y: (scrabble3D.height - about.height) / 2

    component LinkText: Rectangle {
        width: parent.width
        height: theText.height
        clip: true //rich text does not wrap or elide
        color: "transparent"
        property alias text: theText.text
        Text {
            id: theText
            color: config.myPalette.windowText
            text: ""
            linkColor: isDark(config.myPalette.window) ? Qt.lighter("#0000FF") : "#0000FF"
            onLinkActivated: Qt.openUrlExternally(link)
            MouseArea {
                id: maLink
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
            ToolTip {
                text: theText.hoveredLink
                visible: maLink.containsMouse
                delay: 1000
                timeout: 5000
            }
        }
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 8
        columns: 2
        Image {
            Layout.columnSpan: 2
            Layout.preferredWidth: about.width / 2
            Layout.preferredHeight: about.width / 2
            Layout.minimumWidth: 50
            Layout.leftMargin: about.width / 4
            // does not work since layout exceed dialog width
            // do not attempt to bind the width or height of the dialog... https://doc.qt.io/qt-5/qml-qtquick-dialogs-dialog.html
//            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 12
            Layout.bottomMargin: 12
            source: "qrc:///resources/about.png"
        }
        Label { text: qsTr("Version:")    } Label {   text: GamePlay.version() }
        Label { text: qsTr("Author:")     } LinkText{ text: "<a href=\"mailto: heiko_tietze@web.de\">Heiko Tietze</a>" }
        Label { text: qsTr("Manual:")     } LinkText{ text: "<a href=\"http://scrabble.sourceforge.net/wiki/\">Wiki</a> (outdated)" }
        Label { text: qsTr("License:")    } LinkText{ text: "<a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License v3</a>" }
        Label { text: qsTr("Contribute:") } LinkText{ text: "<a href=\"https://github.com/Scrabble3D/4.0\">https://github.com/Scrabble3D/4.0</a>" }
        Label { text: qsTr("Translate:")  } LinkText{ text: "<a href=\"https://www.transifex.com/scrabble3d/\">https://www.transifex.com/scrabble3d/</a>" }
        Label { text: qsTr("Donate:")     } LinkText{ text: "<a href=\"https://www.paypal.com/donate/?hosted_button_id=HLC2XR2AKDGJU\">Paypal</a>,
                                                             <a href=\"bitcoin:3Ab97pTXwAb872zY3Prq3MzU6TCdqdzi6h\">Bitcoin</a>,
                                                             <a href=\"https://www.patreon.com/scrabble3d\">Patreon</a>,
                                                             <a href=\"https://liberapay.com/Scrabble3D\">Liberapay</a>"}
    }
}
