import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: aboutDialog
    visible: false
    title: qsTr("About Scrabble3D")
    standardButtons: Dialog.Ok
    modal: true
    width: 320 > screen.width ? screen.width : 320
    //    width: Math.min(contentWidth + 48, scrabble3D.width)
    //    height: Math.min(contentHeight + 128, scrabble3D.height)
    x: (scrabble3D.width - about.width) / 2
    y: (scrabble3D.height - about.height) / 2

    ColumnLayout {
        anchors.fill: parent
        Image {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 12
            Layout.bottomMargin: 12
            Layout.preferredWidth: aboutDialog.width / 2 // 200 //Math.min(width, scrabble3D.width - colLayout.width)
            Layout.preferredHeight: aboutDialog.width / 2 // 200 //Math.min(width, scrabble3D.width - colLayout.width)
            Layout.minimumWidth: 50
            source: "qrc:///resources/about.png"
        }
        Text { text: "<b>"+qsTr("Version:") + "</b> " + GamePlay.version() }
        LinkText { text: "<b>"+qsTr("Author:")  + "</b> <a href=\"mailto: heiko_tietze@web.de\">Heiko Tietze</a>" }
        LinkText { text: "<b>"+qsTr("Manual:") + "</b> <a href=\"http://scrabble.sourceforge.net/wiki/\">Wiki</a> (outdated)" }
        LinkText { text: "<b>"+qsTr("License:") + "</b> <a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License v3</a>" }
        LinkText { text: "<b>"+qsTr("Contribute:") + "</b> <a href=\"https://github.com/Scrabble3D/4.0\">https://github.com/Scrabble3D/4.0</a>" }
        LinkText { text: "<b>"+qsTr("Translate:") + "</b> <a href=\"https://www.transifex.com/scrabble3d/\">https://www.transifex.com/scrabble3d/</a>" }
        LinkText { text: "<b>"+qsTr("Donate:") + "</b> <a href=\"https://www.paypal.com/donate/?hosted_button_id=HLC2XR2AKDGJU\">Paypal</a>,
                                                            <a href=\"bitcoin:3Ab97pTXwAb872zY3Prq3MzU6TCdqdzi6h\">Bitcoin</a>,
                                                            <a href=\"https://www.patreon.com/scrabble3d\">Patreon</a>,
                                                            <a href=\"https://liberapay.com/Scrabble3D\">Liberapay</a>"}
    }
}
