// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: button

    width: label.width
    height: label.height

    property string labelText: qsTr("Button")
    property color labelColor: "black"
    property color labelHoverColor: labelColor

    signal clicked()

    Text {
        id: label

        text: labelText
        color: labelColor
    }

    MouseArea {
        anchors.fill: parent

        hoverEnabled: true

        onClicked: {
            button.clicked()
        }

        onEntered: {
            label.color = labelHoverColor
        }

        onExited: {
            label.color = labelColor
        }
    }
}
