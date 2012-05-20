// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: base

    BackCoverPage {
        id: backCoverPage

        width: parent.width
        height: parent.height
        visible: false
    }

    GamePage {
        id: gamePage

        width: parent.width
        height: parent.height
        visible: false
    }

    FrontCoverPage {
        id: frontCoverPage

        width: parent.width
        height: parent.height

        transitions:
            Transition {
                from: ""
                to: "hidden"
                SequentialAnimation {
                    PropertyAction {
                        target: gamePage
                        property: "visible"
                        value: true
                    }
                    NumberAnimation {
                        properties: "x"
                        easing.type: Easing.OutElastic
                        duration: 1000
                    }
                    PropertyAction {
                        property: "visible"
                    }
                }
            }
    }
}
