// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
//page settings
Item {
    id: page

    property int baseFontSize: Math.min(width, height) * 0.007

    signal pageRequested(string pageName)
    //page state to show the selected page or hidden the current page
    states: [
        //page visible to true
        State {
            name: "shown"
            PropertyChanges {
                target: page
                x: 0
                visible: true
            }
        },
        //page visible to false
        State{
            name:"hidden"
            PropertyChanges{
            target:page
            visible:false
            }

        },
        //page visible to false by moving left
        State {
            name: "hiddenLeft"
            PropertyChanges {
                target: page
                x: -width
                visible: false
            }
        },
        //page visible to false by moving right
        State {
            name: "hiddenRight"
            PropertyChanges {
                target: page
                x: width
                visible: false
            }
        }
    ]
    //to create page transition animation
    transitions: [
        //transition from state shown to visible
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
        //transition from state shown to hidden left and hidden right
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
        //transition from state hidden left and hidden right to shown
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
