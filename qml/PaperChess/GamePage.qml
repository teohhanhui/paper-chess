// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import CustomComponents 1.0

Page {
    id: page

    Image {
        anchors.fill: parent

        source: "images/paper_texture.png"
        fillMode: Image.Tile
    }

    Column {
        anchors.fill: parent

        //spacing: 5

        Flickable {
            id: flicky

            anchors {
                left: parent.left
                right: parent.right
            }

            height: parent.height - toolbar.height

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

        Flow {
            id: toolbar

            Button {
                color: "transparent"
                labelText: qsTr("Menu")

                onClicked: overlayMenu.state = "shown"
            }
        }
    }

    Column {
        id: overlayMenu

        anchors {
            centerIn: parent
        }
        opacity: 0
        visible: false

        Button {
            labelText: "Main Menu"

            onClicked: pageRequested("mainMenuPage")
        }

        states: [
            State {
                name: "shown"
                PropertyChanges {
                    target: overlayMenu
                    visible: true
                    opacity: 1
                }
            },
            State {
                name: "hidden"
                PropertyChanges {
                    target: overlayMenu
                    opacity: 0
                    visible: false
                }
            }
        ]

        transitions: [
            Transition {
                from: "*"
                to: "shown"
                SequentialAnimation {
                    PropertyAction {
                        properties: "visible"
                    }
                    NumberAnimation {
                        properties: "opacity"
                        easing.type: Easing.InQuad
                        duration: 200
                    }
                }
            },
            Transition {
                from: "shown"
                to: "hidden"
                SequentialAnimation {
                    NumberAnimation {
                        properties: "opacity"
                        easing: Easing.OutQuad
                        duration: 200
                    }
                    PropertyAction {
                        properties: "visible"
                    }
                }
            }
        ]
    }
}
