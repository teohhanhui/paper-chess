import QtQuick 2.9

Rectangle {
    id: playerIndicator

    property alias playerName: playerNameText.text
    property alias playerMarkerSource: playerMarkerImage.source
    property alias font: playerNameText.font
    property color activeColor: "white"

    Text {
        id: playerNameText

        anchors {
            left: parent.left
            right: parent.right
            margins: 3 * baseFontSize
        }

        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }

    Item {
        anchors {
            left: parent.left
            right: parent.right
            top: playerNameText.bottom
            bottom: parent.bottom
        }

        Image {
            id: playerMarkerImage

            anchors.centerIn: parent

            sourceSize.height: font.pixelSize
        }
    }

    states: [
        State {
            name: "active"

            PropertyChanges {
                target: playerIndicator
                color: activeColor
            }

            PropertyChanges {
                target: playerNameText
                color: "black"
                opacity: 1
            }

            PropertyChanges {
                target: playerMarkerImage
                opacity: 1
            }
        },
        State {
            name: "inactive"

            PropertyChanges {
                target: playerIndicator
                color: "transparent"
            }

            PropertyChanges {
                target: playerNameText
                color: "gray"
                opacity: 0.5
            }

            PropertyChanges {
                target: playerMarkerImage
                opacity: 0.5
            }
        }
    ]

    transitions: [
        Transition {
            from: "inactive"
            to: "active"

            ParallelAnimation {
                ColorAnimation {
                    target: playerIndicator
                    properties: "color"
                    easing.type: Easing.InQuad
                    duration: 200
                }

                ColorAnimation {
                    target: playerNameText
                    properties: "color"
                    easing.type: Easing.InQuad
                    duration: 200
                }

                NumberAnimation {
                    target: playerNameText
                    properties: "opacity"
                    easing.type: Easing.InQuad
                    duration: 200
                }

                NumberAnimation {
                    target: playerMarkerImage
                    properties: "opacity"
                    easing.type: Easing.InQuad
                    duration: 200
                }
            }
        },
        Transition {
            from: "active"
            to: "inactive"

            ParallelAnimation {
                ColorAnimation {
                    target: playerIndicator
                    properties: "color"
                    easing.type: Easing.OutQuad
                    duration: 200
                }

                ColorAnimation {
                    target: playerNameText
                    properties: "color"
                    easing.type: Easing.OutQuad
                    duration: 200
                }

                NumberAnimation {
                    target: playerNameText
                    properties: "opacity"
                    easing.type: Easing.OutQuad
                    duration: 200
                }

                NumberAnimation {
                    target: playerMarkerImage
                    properties: "opacity"
                    easing.type: Easing.OutQuad
                    duration: 200
                }
            }
        }
    ]
}
