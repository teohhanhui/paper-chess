// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

CoverPage {
    id: page

    FontLoader {
        id: sketchFont

        source: "fonts/CabinSketch-Regular.ttf"
    }

    Button {
        id: startButton

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: height

        color: "transparent"
        labelText: qsTr("Start") + " >>"
        labelFont {
            family: sketchFont.name
        }
        labelColor: "black"
        labelHoverColor: "#454343" // graphite

        onClicked: pageRequested("gamePage")
    }
}
