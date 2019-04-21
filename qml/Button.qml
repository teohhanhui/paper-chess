import QtQuick 2.9

Rectangle {
    id: button

    property alias text: label.text
    property alias font: label.font
    property alias labelColor: label.color
    property alias labelActiveColor: label.activeColor

    signal clicked()

    width: label.width
    height: label.height

    color: "transparent"

    Text {
        id: label

        property color activeColor: "#454343" // graphite

        color: "black"
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        onClicked: button.clicked()
    }

    states: [
        State {
            name: "active"
            when: mouseArea.containsMouse

            PropertyChanges {
                target: label
                color: label.activeColor
            }
        }
    ]
}
