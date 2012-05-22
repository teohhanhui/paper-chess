// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: button

    property alias labelText: label.text
    property alias labelFont: label.font
    property alias labelColor: label.normalColor
    property alias labelHoverColor: label.hoverColor

    signal clicked()

    width: label.width
    height: label.height

    Text {
        id: label

        property color normalColor
        property color hoverColor

        color: normalColor
    }

    MouseArea {
        anchors.fill: parent

        hoverEnabled: true

        onClicked: {
            button.clicked()
        }

        onEntered: {
            label.color = label.hoverColor
        }

        onExited: {
            label.color = label.normalColor
        }
    }
}
