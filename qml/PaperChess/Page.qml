// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id: page

    signal pageRequested(string pageName)

    states: [
        State {
            name: "shown"
            PropertyChanges {
                target: page
                x: 0
                visible: true
            }
        },
        State {
            name: "hiddenLeft"
            PropertyChanges {
                target: page
                x: -width
                visible: false
            }
        },
        State {
            name: "hiddenRight"
            PropertyChanges {
                target: page
                x: width
                visible: false
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "hiddenLeft, hiddenRight"
            SequentialAnimation {
                NumberAnimation {
                    properties: "x"
                    easing.type: Easing.OutElastic
                    duration: 1000
                }
                PropertyAction {
                    properties: "visible"
                }
            }
        },
        Transition {
            from: "*"
            to: "shown"
            SequentialAnimation {
                PropertyAction {
                    properties: "visible"
                }
                NumberAnimation {
                    properties: "x"
                    easing.type: Easing.OutElastic
                    duration: 1000
                }
            }
        }
    ]
}
