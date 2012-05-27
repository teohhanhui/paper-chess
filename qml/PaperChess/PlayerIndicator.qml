// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1


    Rectangle {
        id: playerIndicator

        property alias playerName: playerNameText.text
        property alias imageSource: playerImage.source

        width: childrenRect.width

        color:"transparent"

        Text {
            id: playerNameText
        }

        Rectangle {
            id: playerStage

            radius:10

            anchors {
                left: parent.left
                right: parent.right
                top: playerNameText.bottom
                bottom: parent.bottom
            }

            color:"transparent"

            Image{
                id: playerImage

                smooth: true
                sourceSize.width: baseFontSize*6
                anchors.centerIn: parent
            }
        }

    states: [
        State {
            name: "active"

                PropertyChanges {
                    target: playerStage
                    color:"#ffae1a"
                    visible: true
                    opacity: 1
                }

                PropertyChanges {
                    target: playerNameText
                    color:"black"
                    visible: true
                    opacity: 1
                }

        },
        State {
            name: "inactive"
                PropertyChanges {
                    target: playerStage
                    color:"transparent"
                    visible: true
                    opacity: 0.5
            }

            PropertyChanges{
                target:playerNameText
                color:"grey"
                visible: true
                opacity: 0.5
            }
        }
    ]


    transitions: [
        Transition {
            from: "inactive"
            to: "active"
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
            from: "active"
            to: "inactive"
            SequentialAnimation {
                NumberAnimation {
                    properties: "opacity"
                    easing.type: Easing.OutQuad
                    duration: 200
                }
                PropertyAction {
                    properties: "visible"
                }
            }
        }
    ]
}
