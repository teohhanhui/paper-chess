// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import PaperChess 1.0

Page {
    id: page
    //properties which store player 1 and player 2 name input
    property string player1Name
    property string player2Name

    //font type
    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
    }
    //load game page background image
    Image {
        anchors.fill: parent

        source: "qrc:/images/wooden_texture.jpg"
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
        //load gameboard background image
        Image {
            anchors.fill: parent

            source: "qrc:/images/paper_texture.png"
            fillMode: Image.Tile
        }
        //set up game engine
        GameBoard {
            id: gameBoard

            anchors.fill: parent

            engine: gameEngine
            dotSources: [
                player1Indicator.playerMarkerSource,
                player2Indicator.playerMarkerSource
            ]
            gridStroke {
                color: "slategray"
                width: 1
            }
        }
        //pinch to zoom function
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
        //mouseArea action
        MouseArea {
            id: touchy

            property variant touchedPoint

            anchors.fill: parent
            //
            onClicked: {
                touchy.touchedPoint = Qt.point(mouseX, mouseY)
                clickTimer.restart()
            }

            onDoubleClicked: {
                clickTimer.stop()

                if (flicky.state === "zoomedIn") {
                    flicky.contentWidth = flicky.width
                    flicky.contentHeight = flicky.height
                }
                else {
                    flicky.resizeContent(flicky.width * pinchy.pinch.maximumScale,
                                         flicky.height* pinchy.pinch.maximumScale,
                                         Qt.point(mouseX, mouseY))
                }
            }
            //set a timer to prevent double clicked
            Timer {
                id: clickTimer

                interval: doubleClickInterval

                onTriggered: gameBoard.markPosition(touchy.touchedPoint)
            }
        }
        //game board zoom states
        states: [
            State {
                name: "zoomedIn"
                when: flicky.contentWidth > flicky.width
            },
            State {
                name: "zoomedOut"
                when: flicky.contentWidth <= flicky.width
            }
        ]
    }
    //upper status bar
    Rectangle {
        id: gamebar

        height: 20 * baseFontSize
        anchors {
            left: parent.left
            right: parent.right
        }
        //background gradient color
        gradient: Gradient {
            GradientStop { position: 1.0; color: "#d5a95e" }
            GradientStop { position: 0.0; color: "#b78530" }
        }
        //player 1's turn indicator
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
            playerMarkerSource: "qrc:/images/dot.svg"
            fontSize: 7 * baseFontSize
            activeColor: stageBar.color
        }
        //game steps status bar
        Rectangle {
           id: turnsLeftBar

           height: parent.height * 0.5
           anchors {
               left: player1Indicator.right
               right: player2Indicator.left
           }

           color: "transparent"

           Text {
               anchors.centerIn: parent

               text: "Moves: " + gameEngine.turnsLeft
               font.pixelSize: 6 * baseFontSize
           }
        }
        //player's action status bar
        Rectangle {
            id: stageBar

            anchors {
                left: player1Indicator.right
                right: player2Indicator.left
                top: turnsLeftBar.bottom
                bottom: parent.bottom
            }

            color: Qt.rgba(255, 128, 0, 0.25)

            Text {
                function getStageName(stage) {
                    switch(stage) {
                    case GameEngine.PlaceDotStage:
                        return "Place Dot"
                    case GameEngine.ConnectingStage:
                        return "Connect Dots"
                    default:
                        return ""
                    }
                }

                anchors.centerIn: parent

                text: getStageName(gameEngine.stage)
                font.pixelSize: 8 * baseFontSize
                color: "white"
            }
        }
        //player 2's turn indicator
        PlayerIndicator {
            id: player2Indicator

            width: 35 * baseFontSize
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            state: "inactive"

            playerName: player2Name
            playerMarkerSource: "qrc:/images/cross.svg"
            fontSize: 7 * baseFontSize
            activeColor: stageBar.color
        }
    }
    //bottom menu toolbar
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
            GradientStop { position: 0.5; color: "#b78530" }
        }
        //menu button to show the the overlay menu
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
        //end player's turn button
        Button {
            id: endTurnButton

            visible: gameEngine.stage === GameEngine.ConnectingStage
            anchors {
                right: parent.right
                bottom: parent.bottom
                margins: 2 * baseFontSize
            }

            text: qsTr("End Turn")
            font.pixelSize: 8 * baseFontSize            

            onClicked: {
                gameEngine.endTurn()

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
    //overlay menu to resume game or exit game
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
            width: 60 * baseFontSize
            height: 55 * baseFontSize
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
                        pixelSize: 12 * baseFontSize
                    }

                    onClicked: overlayMenu.state = "hidden"
                }

                Button {
                    text: "End Game"
                    font {
                        family: handwritingFont.name
                        pixelSize: 12 * baseFontSize
                    }

                    onClicked: promptBox.state = "shown"
                }
            }
        }
        //exit game confirm box
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
                        pixelSize: 10 * baseFontSize
                    }
                    wrapMode: Text.WordWrap
                }

                Button {
                    anchors {
                        left: parent.left
                        bottom: parent.bottom
                    }
                    //exit to main menu
                    text: "Yes"
                    font {
                        family: handwritingFont.name
                        pixelSize: 11 * baseFontSize
                    }

                    onClicked: {
                        pageRequested("scorePage")
                        overlayMenu.state = "hidden"
                        promptBox.state = "hidden"
                    }
                }
                //resume game
                Button {
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                        leftMargin: 10
                        rightMargin: 10
                    }

                    text: "No"
                    font {
                        family: handwritingFont.name
                        pixelSize: 11 * baseFontSize
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
