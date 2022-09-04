import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    visible: false
    title: qsTr("About Scrabble3D")
    standardButtons: Dialog.Ok
    modal: true
    width: Math.min(contentWidth + 48, app.width)
    height: Math.min(contentHeight + 128, app.height)
    x: (app.width - about.width) / 2
    y: (app.height - about.height) / 2

    RowLayout {
        spacing: 24
        Image {
            Layout.leftMargin: 24
            Layout.topMargin: 24
            Layout.preferredWidth: Math.min(width, app.width - colLayout.width)
            Layout.minimumWidth: 50
            fillMode: Image.PreserveAspectFit
            source: "qrc:///resources/about.png"
        }
        ColumnLayout {
            id: colLayout
            Layout.topMargin: 24
            Layout.alignment: Qt.AlignTop
            Text { color: config.myPalette.windowText; text: "<b>"+qsTr("Version:") + "</b> " + GamePlay.version() }
            Text { color: config.myPalette.windowText; text: "<b>"+qsTr("Author:")  + "</b> Heiko Tietze" }
            LinkText { text: "<b>"+qsTr("Manual:") + "</b> <a href=\"http://scrabble.sourceforge.net/wiki/\">Wiki</a> (outdated)" }
            LinkText { text: "<b>"+qsTr("License:") + "</b> <a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License v3</a>" }
            LinkText { text: "<b>"+qsTr("Contribute:") + "</b> <a href=\"https://github.com/Scrabble3D/4.0\">https://github.com/Scrabble3D/4.0</a>" }
            LinkText { text: "<b>"+qsTr("Translate:") + "</b> to be done" }//"</b> <a href=\"https://www.transifex.com/scrabble3d/\">https://www.transifex.com/scrabble3d/</a>" }
            LinkText { text: "<b>"+qsTr("Donate:") + "</b> <a href=\"https://www.paypal.com/donate/?hosted_button_id=HLC2XR2AKDGJU\">Paypal</a>,
                                                            <a href=\"bitcoin:3Ab97pTXwAb872zY3Prq3MzU6TCdqdzi6h\">Bitcoin</a>,
                                                            <a href=\"https://www.patreon.com/scrabble3d\">Patreon</a>"}
        }
    }
}
