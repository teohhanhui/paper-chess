// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id: page

    property int baseFontSize: Math.min(width, height) * 0.007

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
        State{
            name:"hidden"
            PropertyChanges{
            target:page
            visible:false
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
            from: "shown"
            to: "hidden"
            SequentialAnimation {
                NumberAnimation {
                    properties: "x"
                    easing.type: Easing.OutQuart
                    duration: 600
                }
                PropertyAction {
                    properties: "visible"
                }
            }
        },
        Transition {
            from: "shown"
            to: "hiddenLeft, hiddenRight"
            SequentialAnimation {
                NumberAnimation {
                    properties: "x"
                    easing.type: Easing.OutQuart
                    duration: 600
                }
                PropertyAction {
                    properties: "visible"
                }
            }
        },
        Transition {
            from: "hiddenLeft, hiddenRight"
            to: "shown"
            SequentialAnimation {
                PropertyAction {
                    properties: "visible"
                }
                NumberAnimation {
                    properties: "x"
                    easing.type: Easing.InQuart
                    duration: 600
                }
            }
        }
    ]
}
