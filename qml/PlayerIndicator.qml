// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: playerIndicator

    property alias playerName: playerNameText.text
    property alias playerMarkerSource: playerMarkerImage.source
    property alias fontSize: playerNameText.font.pixelSize
    property color activeColor: "white"

    Text {
        id: playerNameText

        anchors.horizontalCenter: parent.horizontalCenter
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
            smooth: true

            sourceSize.height: fontSize
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
