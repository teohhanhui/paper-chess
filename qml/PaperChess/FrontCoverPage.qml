// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

CoverPage {
    id: page

    states:
        State {
            name: "hidden"
            PropertyChanges {
                target: page
                x: -width
                visible: false
            }
        }

    Button {
        id: startButton

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: height

        color: "transparent"
        labelText: qsTr("Start") + " >>"
        labelColor: "black"
        labelHoverColor: "#454343" // graphite

        onClicked: {
            page.state = "hidden"
        }
    }
}
