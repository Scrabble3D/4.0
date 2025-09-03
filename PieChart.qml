import QtQuick 2.0
// based on https://www.ics.com/blog/creating-qml-controls-scratch-piechart

Canvas {
    id: root

 // public
    property variant points: []

 // private
    onPointsChanged: requestPaint()

    property double iconSize: 16

    onPaint: {
        var context = getContext("2d")
        var start   = Math.PI / points.length
        var radius  = iconSize / 2
        var center  = Qt.vector2d(width / 2, height / 2)

        context.clearRect(0, 0, iconSize, iconSize)

        for(var i = 0; i < points.length; i++) {
            var end  = start + 2 * Math.PI / points.length

            context.fillStyle = points[i]
            context.beginPath()
            context.arc(center.x, center.y, radius, start, end)
            context.lineTo(center.x, center.y)
            context.fill()
            context.strokeStyle = palette.button
            context.stroke()
            context.closePath()

            start = end
        }
    }
}
