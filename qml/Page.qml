import QtQuick 2.9

Item {
    id: page

    property int baseFontSize: Math.min(width, height) * 0.007
    property int transitionDuration: width * 1.5

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
            name: "hidden"
            PropertyChanges {
                target: page
                opacity: 0
                visible: false
            }
        },
        State {
            name: "hiddenLeft"
            PropertyChanges {
                target: page
                x: -width - 20 * baseFontSize
                visible: false
            }
        },
        State {
            name: "hiddenRight"
            PropertyChanges {
                target: page
                x: width + 20 * baseFontSize
                visible: false
            }
        }
    ]

    transitions: [
        Transition {
            from: "shown"
            to: "hiddenLeft"
            SequentialAnimation {
                NumberAnimation {
                    properties: "x"
                    easing.type: Easing.InOutQuart
                    duration: transitionDuration
                }
                PropertyAction {
                    properties: "visible"
                }
            }
        },
        Transition {
            from: "shown"
            to: "hiddenRight"
            SequentialAnimation {
                NumberAnimation {
                    properties: "x"
                    easing.type: Easing.InQuart
                    duration: transitionDuration
                }
                PropertyAction {
                    properties: "visible"
                }
            }
        },
        Transition {
            from: "hiddenLeft"
            to: "shown"
            SequentialAnimation {
                PropertyAction {
                    properties: "visible"
                }
                NumberAnimation {
                    properties: "x"
                    easing.type: Easing.InOutQuart
                    duration: transitionDuration
                }
            }
        },
        Transition {
            from: "hiddenRight"
            to: "shown"
            SequentialAnimation {
                PropertyAction {
                    properties: "visible"
                }
                NumberAnimation {
                    properties: "x"
                    easing.type: Easing.InOutQuart
                    duration: transitionDuration
                }
            }
        }
    ]
}
