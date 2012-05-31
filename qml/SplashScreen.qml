// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: screen

    property int baseFontSize: Math.min(width, height) * 0.007

    Image {
        anchors.fill: parent

        source: "qrc:/images/orange_gradient.svg"
    }

    Image {
        id: icon

        anchors.centerIn: parent

        source: "qrc:/images/logo_icon.svg"
        sourceSize.width: 100 * baseFontSize
    }

    Image {
        anchors {
            horizontalCenter: icon.horizontalCenter
            top: icon.bottom
            margins: height / 2
        }

        source: "qrc:/images/logo_text.png"
        sourceSize.width: icon.width * 1.1
        fillMode: Image.PreserveAspectFit
    }

    MouseArea {
        anchors.fill: parent

        onClicked: screen.state = "hidden"
    }

    Timer {
        interval: 1500
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
