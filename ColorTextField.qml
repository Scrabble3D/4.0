import QtQuick
import QtQuick.Controls

TextField {
    id: control
    implicitWidth: 60 + 28
//    implicitHeight: 28 //BUG: colortextfield: height on Android

    property bool accepted

    validator: RegularExpressionValidator {
        regularExpression: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d)$/
    } //accept from 00:00:00 to 23:59:59

    color: enabled ? "black" : "darkgrey"

    background: Rectangle {
        color: control.enabled
               ? (control.accepted)
                  ? "red" : "white"
               : "lightgrey"
    }

}
