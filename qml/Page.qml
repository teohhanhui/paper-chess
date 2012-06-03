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
                opacity: 1
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: page
                opacity: 0
            }
        },
        State {
            name: "hiddenLeft"
            PropertyChanges {
                target: page
                x: -width - 20 * baseFontSize
            }
        },
        State {
            name: "hiddenRight"
            PropertyChanges {
                target: page
                x: width
            }
        }
    ]

    transitions: [
        Transition {
            from: "shown"
            to: "hidden"
            NumberAnimation {
                properties: "opacity"
                easing.type: Easing.InExpo
                duration: 600
            }
        },
        Transition {
            from: "shown"
            to: "hiddenLeft"
            NumberAnimation {
                properties: "x"
                easing.type: Easing.InOutQuart
                duration: 600
            }
        },
        Transition {
            from: "shown"
            to: "hiddenRight"
            NumberAnimation {
                properties: "x"
                easing.type: Easing.InQuart
                duration: 600
            }
        },
        Transition {
            from: "hiddenLeft"
            to: "shown"
            NumberAnimation {
                properties: "x"
                easing.type: Easing.InOutQuart
                duration: 600
            }
        },
        Transition {
            from: "hiddenRight"
            to: "shown"
            NumberAnimation {
                properties: "x"
                easing.type: Easing.InOutQuart
                duration: 600
            }
        }
    ]
}
