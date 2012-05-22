// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: screen

    Image {
        anchors.fill: parent

        source: "images/splash_gradient.svg"
    }

    Image {
        id: icon

        anchors.centerIn: parent

        source: "images/splash_icon.svg"
    }

    Image {
        anchors {
            horizontalCenter: icon.horizontalCenter
            top: icon.bottom
            margins: 10
        }

        source: "images/splash_text.png"
        sourceSize.width: icon.width * 1.1
        fillMode: Image.PreserveAspectFit
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            screen.state = "hidden";
        }
    }

    Timer {
        interval: 2000
        running: true

        onTriggered: {
            screen.state = "hidden"
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: screen
                opacity: 0
                visible: false
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "hidden"
            SequentialAnimation {
                NumberAnimation {
                    properties: "opacity"
                    duration: 100
                }
                PropertyAction {
                    properties: "visible"
                }
            }
        }
    ]
}
