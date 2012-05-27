// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import CustomComponents 1.0

Page {
    id: page

    property string player1Name
    property string player2Name

    FontLoader {
        id: handwritingFont

        source: "fonts/CoveredByYourGrace.ttf"
    }

    Image {
        anchors.fill: parent

        source: "images/paper_texture.png"
        fillMode: Image.Tile
    }



    Flickable {
        id: flicky

        anchors {
            left: parent.left
            right: parent.right
            top: gamebar.bottom
            bottom: toolbar.top
        }

        contentWidth: width
        contentHeight: height

        GameGrid {
            id: gameGrid

            anchors.fill: parent

            rows: 40
            columns: 20
            stroke.color: "slategray"
            stroke.width: 1
        }

        PinchArea {
            id: pinchy

            anchors.fill: parent

            pinch.minimumScale: 1.0
            pinch.maximumScale: 5.0
            pinch.dragAxis: Pinch.NoDrag

            property real contentScale: 1.0
            property real initialScale

            onPinchStarted: {
                initialScale = contentScale
            }

            onPinchUpdated: {
                var newScale = initialScale * pinch.scale

                if (newScale < pinchy.pinch.minimumScale)
                    newScale = pinchy.pinch.minimumScale

                if (newScale > pinchy.pinch.maximumScale)
                    newScale = pinchy.pinch.maximumScale

                contentScale = newScale

                flicky.resizeContent(flicky.width * newScale, flicky.height * newScale, pinch.center)
            }

            onPinchFinished: {
                flicky.returnToBounds()
            }
        }

        MouseArea {
            id: touchy

            anchors.fill: parent

            onPressAndHold: {
                //
            }

            onDoubleClicked: {
                flicky.contentWidth = flicky.width
                flicky.contentHeight = flicky.height
            }
        }
    }

    Rectangle {
        id: gamebar
        radius:2.5

        height: baseFontSize * 20
        anchors {
            left: parent.left
            right: parent.right
        }

        gradient: Gradient {
                 GradientStop { position: 0.0; color: "#d5a95e" }
                 GradientStop { position: 1.0; color: "#b78530" }
             }

        Rectangle {
            id: player1Bar
            width: childrenRect.width
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                leftMargin: 5
            }

            color:"transparent"

            Text {
                id: player1Text
                text: player1Name
            }

            Rectangle {
                id: player1Stage

                radius:10

                anchors {
                    left: parent.left
                    right: parent.right
                    top: player1Text.bottom
                    bottom: parent.bottom
                }

                Image{
                    source: "images/dot.svg"
                    smooth: true
                    sourceSize.width: baseFontSize*6
                    anchors.centerIn: parent
                }
            }
        }



        Rectangle {
           id:stepsBar

           //property int steps:200

           width: childrenRect.width
           height: childrenRect.height
           anchors {
               horizontalCenter: parent.horizontalCenter
           }

           color:"transparent"

           Text {
               text: "Steps: " //+ steps
           }
        }

        Rectangle {
            id: stageBar

            color:"transparent"

            anchors {
                left: player1Bar.right
                right: player2Bar.left
                top: stepsBar.bottom
                bottom: parent.bottom
            }

            Text{
                text:"Place Dot"
                anchors.centerIn: parent
            }
        }



        Rectangle {
            id:player2Bar
            width:childrenRect.width
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
                rightMargin:5
            }

            color:"transparent"

            Text {
                id:player2Text
                text: player2Name
            }

            Rectangle {
                id: player2Stage

                radius:10

                anchors {
                    left: parent.left
                    right: parent.right
                    top: player2Text.bottom
                    bottom: parent.bottom
                }

                Image{
                    source:"images/cross.svg"
                    smooth:true
                    sourceSize.width: baseFontSize*6
                    anchors.centerIn: parent
                }
            }
        }

    }
    Rectangle {
        id: toolbar
        height: baseFontSize * 12

        radius:2.5


        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right

        }

        gradient: Gradient {
                 GradientStop { position: 0.0; color: "#d5a95e" }
                 GradientStop { position: 1.0; color: "#b78530" }
             }

        Button {

            anchors {
                left:parent.left
                top: parent.top
                bottom: parent.bottom
                leftMargin: 5
            }
            color: "transparent"
            text: qsTr("Menu")

            onClicked: overlayMenu.state = "shown"
        }

        Button{

            anchors {
                right:parent.right
                top: parent.top
                bottom: parent.bottom
                rightMargin: 5
            }

            color: "transparent"
            text: qsTr("End Turn")

            onClicked:{
                //if()
            }

        }
    }

    Rectangle{

        id: overlayMenu

        width: 50 * baseFontSize
        height: 40 * baseFontSize

        border.color: "black"
        border.width: 2
        gradient: Gradient {
                 GradientStop { position: 0.0; color: "#d5a95e" }
                 GradientStop { position: 1.0; color: "#b78530" }
             }

        radius:10

        opacity: 0
        visible: false


        anchors {
            centerIn: parent

        }

    Column {
        id: overlayContent

        anchors.fill:parent
        anchors.margins:10 * baseFontSize

            Button {
                id: title
                color: "transparent"
                text: "Resume"
                font {
                    family: handwritingFont.name
                    pixelSize: 9 * baseFontSize
                }

                onClicked: overlayMenu.state = "hidden"
            }

            Button {
                text: "End Game"
                color: "transparent"
                font {
                    family: handwritingFont.name
                    pixelSize: 9 * baseFontSize
                }
                onClicked: pageRequested("mainMenuPage")
            }

       }

        states: [
            State {
                name: "shown"
                PropertyChanges {
                    target: overlayMenu
                    visible: true
                    opacity: 1
                }
            },
            State {
                name: "hidden"
                PropertyChanges {
                    target: overlayMenu
                    opacity: 0
                    visible: false
                }
            }
        ]


        transitions: [
            Transition {
                from: "*"
                to: "shown"
                SequentialAnimation {
                    PropertyAction {
                        properties: "visible"
                    }
                    NumberAnimation {
                        properties: "opacity"
                        easing.type: Easing.InQuad
                        duration: 200
                    }
                }
            },
            Transition {
                from: "shown"
                to: "hidden"
                SequentialAnimation {
                    NumberAnimation {
                        properties: "opacity"
                        easing.type: Easing.OutQuad
                        duration: 200
                    }
                    PropertyAction {
                        properties: "visible"
                    }
                }
            }
        ]
    }

}

