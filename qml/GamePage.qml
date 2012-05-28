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

        source: "images/rainbow_gradient.svg"
        sourceSize.width: parent.width
        fillMode: Image.Stretch
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

        Image {
            anchors.fill: parent

            source: "images/paper_texture.png"
            fillMode: Image.Tile
        }

        GameBoard {
            id: gameBoard

            anchors.fill: parent

            rows: 40
            columns: 25
            gridStroke {
                color: "slategray"
                width: 1
            }
        }

        PinchArea {
            id: pinchy

            property real contentScale: 1.0
            property real initialScale

            anchors.fill: parent

            pinch {
                minimumScale: 1.0
                maximumScale: 5.0
                dragAxis: Pinch.NoDrag
            }

            onPinchStarted: initialScale = contentScale

            onPinchUpdated: {
                var newScale = initialScale * pinch.scale

                if (newScale < pinchy.pinch.minimumScale)
                    newScale = pinchy.pinch.minimumScale

                if (newScale > pinchy.pinch.maximumScale)
                    newScale = pinchy.pinch.maximumScale

                contentScale = newScale

                flicky.resizeContent(flicky.width * newScale, flicky.height * newScale, pinch.center)
            }

            onPinchFinished: flicky.returnToBounds()
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

        height: 20 * baseFontSize
        anchors {
            left: parent.left
            right: parent.right
        }

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#d5a95e" }
            GradientStop { position: 1.0; color: "#b78530" }
        }

        PlayerIndicator {
            id: player1Indicator

            width: 40 * baseFontSize
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            state: "active"

            playerName: player1Name
            playerMarkerSource: "images/dot.svg"
            fontSize: 7 * baseFontSize
            activeColor: stageBar.color
        }

        Rectangle {
           id: stepsBar

           property int steps: 200

           height: parent.height * 0.5
           anchors {
               left: player1Indicator.right
               right: player2Indicator.left
           }

           color: "transparent"

           Text {
               anchors.centerIn: parent

               text: "Steps: " + stepsBar.steps
               font.pixelSize: 6 * baseFontSize
           }
        }

        Rectangle {
            id: stageBar

            anchors {
                left: player1Indicator.right
                right: player2Indicator.left
                top: stepsBar.bottom
                bottom: parent.bottom
            }

            //color: "#ffae1a"
            color: Qt.rgba(255, 128, 0, 0.25)
            //color: Qt.rgba(90, 65, 24, 0.25)

            Text {
                anchors.centerIn: parent

                text: "Place Dot"
                font.pixelSize: 8 * baseFontSize
            }
        }

        PlayerIndicator {
            id: player2Indicator

            width: 40 * baseFontSize
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            state: "inactive"

            playerName: player2Name
            playerMarkerSource: "images/cross.svg"
            fontSize: 7 * baseFontSize
            activeColor: stageBar.color
        }
    }

    Rectangle {
        id: toolbar

        height: 12 * baseFontSize
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#d5a95e" }
            GradientStop { position: 1.0; color: "#b78530" }
        }

        Button {
            anchors {
                left: parent.left
                bottom: parent.bottom
                margins: 2 * baseFontSize
            }

            text: qsTr("Menu")
            font.pixelSize: 8 * baseFontSize

            onClicked: overlayMenu.state = "shown"
        }

        Button {
            id: endTurnButton

            anchors {
                right: parent.right
                bottom: parent.bottom
                margins: 2 * baseFontSize
            }

            text: qsTr("End Turn")
            font.pixelSize: 8 * baseFontSize

            onClicked: {
                if (player1Indicator.state === "active") {
                    player1Indicator.state = "inactive"
                    player2Indicator.state = "active"
                }
                else {
                    player1Indicator.state = "active"
                    player2Indicator.state = "inactive"
                }
            }
        }
    }

    Item {
        id: overlayMenu

        anchors.fill: parent
        state: "hidden"

        Rectangle {
            anchors.fill: parent
            opacity: 0.5

            color: "black"

            MouseArea {
                anchors.fill: parent
                onClicked: mouse.accepted = true
            }
        }

        Rectangle {
            id: overlayMenuContent

            width: 50 * baseFontSize
            height: 50 * baseFontSize
            anchors.centerIn: parent
            smooth: true

            radius: 10
            border {
                color: "black"
                width: 2
            }
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#d5a95e" }
                GradientStop { position: 1.0; color: "#b78530" }
            }

            Column {
                anchors {
                    fill: parent
                    margins: 10 * baseFontSize
                }

                Button {
                    text: "Resume"
                    font {
                        family: handwritingFont.name
                        pixelSize: 9 * baseFontSize
                    }

                    onClicked: overlayMenu.state = "hidden"
                }

                Button {
                    text: "End Game"
                    font {
                        family: handwritingFont.name
                        pixelSize: 9 * baseFontSize
                    }

                    onClicked: promptBox.state = "shown"
                }
            }
        }

        Rectangle {
            id: promptBox

            width: 100 * baseFontSize
            height: 60 * baseFontSize
            anchors.centerIn: parent
            smooth: true
            state: "hidden"

            radius:10
            border {
                color: "black"
                width: 2
            }
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#d5a95e" }
                GradientStop { position: 1.0; color: "#b78530" }
            }

            Item {
                anchors {
                    fill: parent
                    margins: 5 * baseFontSize
                }

                Text {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    text: "Are you sure you want to exit?"
                    font {
                        family: handwritingFont.name
                        pixelSize: 9 * baseFontSize
                    }
                    wrapMode: Text.WordWrap
                }

                Button {
                    anchors {
                        left: parent.left
                        bottom: parent.bottom
                    }

                    text: "Yes"
                    font {
                        family: handwritingFont.name
                        pixelSize: 9 * baseFontSize
                    }

                    onClicked: {
                        pageRequested("mainMenuPage")
                        overlayMenu.state = "hidden"
                        promptBox.state = "hidden"
                    }
                }

                Button {
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                    }

                    text: "No"
                    font {
                        family: handwritingFont.name
                        pixelSize: 9 * baseFontSize
                    }

                    onClicked: promptBox.state = "hidden"
                }
            }

            states: [
                State {
                    name: "shown"

                    PropertyChanges {
                        target: promptBox
                        visible: true
                        opacity: 1
                    }
                },
                State {
                    name: "hidden"

                    PropertyChanges {
                        target: promptBox
                        opacity: 0
                        visible: false
                    }
                }
            ]

            transitions: [
                Transition {
                    from: "hidden"
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

        states: [
            State {
                name: "shown"

                PropertyChanges {
                    target: overlayMenuContent
                    visible: true
                }

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
                from: "hidden"
                to: "shown"

                SequentialAnimation {
                    PropertyAction {
                        target: overlayMenuContent
                        properties: "visible"
                    }

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
