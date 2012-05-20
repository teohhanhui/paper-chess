// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import CustomComponents 1.0

Rectangle {
    id: page

    color: "beige"

    Image {
        anchors.fill: parent

        source: "paper_texture.png"
        fillMode: Image.Tile
    }

    Flickable {
        id: flicky

        anchors.fill: parent

        contentWidth: width
        contentHeight: height

        GameGrid {
            id: gameGrid

            anchors.fill: parent

            rows: 40
            columns: 20
            stroke.color: "slategray"
            stroke.width: 1
        }

        PinchArea {
            id: pinchy

            anchors.fill: parent

            pinch.minimumScale: 1.0
            pinch.maximumScale: 5.0
            pinch.dragAxis: Pinch.NoDrag

            property real contentScale: 1.0
            property real initialScale

            onPinchStarted: {
                initialScale = contentScale
            }

            onPinchUpdated: {
                var newScale = initialScale * pinch.scale

                if (newScale < pinchy.pinch.minimumScale)
                    newScale = pinchy.pinch.minimumScale

                if (newScale > pinchy.pinch.maximumScale)
                    newScale = pinchy.pinch.maximumScale

                contentScale = newScale
                flicky.resizeContent(flicky.width * newScale, flicky.height * newScale, pinch.center)
            }

            onPinchFinished: {
                flicky.returnToBounds()
            }
        }

        MouseArea {
            id: touchy

            anchors.fill: parent

            onPressAndHold: {
                //
            }

            onDoubleClicked: {
                flicky.contentWidth = flicky.width
                flicky.contentHeight = flicky.height
            }
        }
    }
}
