import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    title: qsTr("About Scrabble3D")
    standardButtons: Dialog.Close
    modal: true
    width: 320 > screen.width ? screen.width : 320
    //    width: Math.min(contentWidth + 48, scrabble3D.width)
    //    height: Math.min(contentHeight + 128, scrabble3D.height)
    x: (scrabble3D.width - about.width) / 2
    y: (scrabble3D.height - about.height) / 2

    palette: config.myPalette

    ColumnLayout {
        anchors.fill: parent
        Image {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 12
            Layout.bottomMargin: 12
            Layout.preferredWidth: about.width / 2 // 200 //Math.min(width, scrabble3D.width - colLayout.width)
            Layout.preferredHeight: about.width / 2 // 200 //Math.min(width, scrabble3D.width - colLayout.width)
            Layout.minimumWidth: 50
            source: "qrc:///resources/about.png"
        }
        LinkText { text: qsTr("Version:") + " " + GamePlay.version() }
        LinkText { text: qsTr("Author:")  + " <a href=\"mailto: heiko_tietze@web.de\">Heiko Tietze</a>" }
        LinkText { text: qsTr("Manual:") + " <a href=\"http://scrabble.sourceforge.net/wiki/\">Wiki</a> (outdated)" }
        LinkText { text: qsTr("License:") + " <a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License v3</a>" }
        LinkText { text: qsTr("Contribute:") + " <a href=\"https://github.com/Scrabble3D/4.0\">https://github.com/Scrabble3D/4.0</a>" }
        LinkText { text: qsTr("Translate:") + " <a href=\"https://www.transifex.com/scrabble3d/\">https://www.transifex.com/scrabble3d/</a>" }
        LinkText { text: qsTr("Donate:") + " <a href=\"https://www.paypal.com/donate/?hosted_button_id=HLC2XR2AKDGJU\">Paypal</a>,
                                                            <a href=\"bitcoin:3Ab97pTXwAb872zY3Prq3MzU6TCdqdzi6h\">Bitcoin</a>,
                                                            <a href=\"https://www.patreon.com/scrabble3d\">Patreon</a>,
                                                            <a href=\"https://liberapay.com/Scrabble3D\">Liberapay</a>"}
    }
}
