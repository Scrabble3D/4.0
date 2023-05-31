import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    title: qsTr("About Scrabble3D")
    standardButtons: Dialog.Close
    modal: true
    clip: true
    width: 380 > screen.width ? screen.width : 380

    x: (scrabble3D.width - about.width) / 2
    y: (scrabble3D.height - about.height) / 2

    GridLayout {
        anchors.fill: parent
        columns: 2
        Image {
            Layout.columnSpan: 2
            Layout.leftMargin: about.width / 4 // Layout.alignment: Qt.AlignHCenter does not work since layout exceed dialog width
            Layout.topMargin: 12
            Layout.bottomMargin: 12
            Layout.preferredWidth: about.width / 2
            Layout.preferredHeight: about.width / 2
            Layout.minimumWidth: 50
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
