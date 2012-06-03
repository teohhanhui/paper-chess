// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import PaperChess 1.0

Page {
    id: page

    property string player1Name
    property string player2Name

    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
    }

    Image {
        anchors.fill: parent

        source: "qrc:/images/wooden_texture.jpg"
        sourceSize.height: parent.height
        fillMode: Image.PreserveAspectCrop
    }

    Flickable {
        id: flicky

        property real contentScale: 1.0

        function scaleContent(newScale, center) {
            flicky.resizeContent(flicky.width * newScale, flicky.height * newScale, center)
            contentScale = newScale
        }

        anchors {
            left: parent.left
            right: parent.right
            top: gamebar.bottom
            bottom: toolbar.top
        }
        clip: true

        contentWidth: width
        contentHeight: height

        onWidthChanged: contentScale = 1

        onMovementEnded: touchShadow.visible = false
        onFlickStarted: touchShadow.visible = false

        Image {
            anchors.fill: parent

            source: "qrc:/images/paper_texture.png"
            fillMode: Image.Tile
        }

        GameBoard {
            id: gameBoard

            anchors.fill: parent
            smooth: true

            engine: gameEngine
            dotSources: [
                player1Indicator.playerMarkerSource,
                player2Indicator.playerMarkerSource
            ]
            markStrokes: [
                Stroke {
                    color: "blue"
                    width: 2
                },
                Stroke {
                    color: "red"
                    width: 2
                }
            ]
            gridStroke {
                color: "slategray"
                width: 1
            }
        }

        PinchArea {
            id: pinchy

            property real initialScale

            anchors.fill: parent

            pinch {
                minimumScale: 1.0
                maximumScale: 7.5
                dragAxis: Pinch.NoDrag
            }

            onPinchStarted: initialScale = flicky.contentScale

            onPinchUpdated: {
                var newScale = initialScale * pinch.scale

                if (newScale < pinchy.pinch.minimumScale)
                    newScale = pinchy.pinch.minimumScale

                if (newScale > pinchy.pinch.maximumScale)
                    newScale = pinchy.pinch.maximumScale

                flicky.scaleContent(newScale, pinch.center)
            }

            onPinchFinished: flicky.returnToBounds()
        }

        MouseArea {
            id: touchy

            property variant touchedPoint

            anchors.fill: parent

            onPressed: {
                touchy.touchedPoint = Qt.point(mouseX, mouseY)
                touchShadow.x = mouseX - touchShadow.width  * 0.5
                touchShadow.y = mouseY - touchShadow.height * 0.5
                touchShadow.visible = true
            }

            onReleased: touchShadow.visible = false

            onClicked: gameBoard.markPosition(touchy.touchedPoint)

            onDoubleClicked: {
                var newScale

                if (flicky.state === "zoomedIn") {
                    newScale = pinchy.pinch.minimumScale
                    flicky.contentWidth = newScale * flicky.width
                    flicky.contentHeight = newScale * flicky.height
                    flicky.contentScale = newScale
                }
                else {
                    newScale = pinchy.pinch.maximumScale
                    flicky.scaleContent(newScale, touchy.touchedPoint)
                }
            }

            Image {
                id: touchShadow

                smooth: true
                visible: false

                source: "qrc:/images/touch_shadow.svg"
                sourceSize.width: 90 * baseFontSize
            }
        }

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

    Rectangle {
        id: gamebar

        height: 20 * baseFontSize
        anchors {
            left: parent.left
            right: parent.right
        }

        gradient: Gradient {
            GradientStop { position: 1.0; color: "#d5a95e" }
            GradientStop { position: 0.0; color: "#b78530" }
        }

        PlayerIndicator {
            id: player1Indicator

            width: 35 * baseFontSize
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

               text: qsTr("Moves: ") + gameEngine.turnsLeft
               font.pixelSize: 6 * baseFontSize
           }
        }

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
                        return qsTr("Place Dot")
                    case GameEngine.ConnectDotsStage:
                        return qsTr("Connect Dots")
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

    Rectangle {
        id: toolbar

        height: 16 * baseFontSize
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#d5a95e" }
            GradientStop { position: 0.5; color: "#b78530" }
        }

        Button {
            anchors {
                left: parent.left
                bottom: parent.bottom
                leftMargin: 4 * baseFontSize
                bottomMargin: 2 * baseFontSize
            }

            text: qsTr("Menu")
            font.pixelSize: 10 * baseFontSize

            onClicked: overlayMenu.state = "shown"
        }

        Button {
            id: confirmMoveButton

            function getActionName(stage) {
                switch(stage) {
                case GameEngine.PlaceDotStage:
                    return qsTr("Place")
                case GameEngine.ConnectDotsStage:
                    return qsTr("Connect")
                default:
                    return ""
                }
            }

            anchors {
                right: parent.right
                bottom: parent.bottom
                rightMargin: 4 * baseFontSize
                bottomMargin: 2 * baseFontSize
            }
            visible: gameBoard.hasPendingMoves

            text: getActionName(gameEngine.stage)
            font.pixelSize: 10 * baseFontSize

            onClicked: gameBoard.acceptMove(true)
        }

        Button {
            id: cancelMoveButton

            anchors {
                right: confirmMoveButton.left
                bottom: parent.bottom
                rightMargin: 10 * baseFontSize
                bottomMargin: 2 * baseFontSize
            }
            visible: gameBoard.hasPendingMoves

            text: qsTr("Cancel")
            font.pixelSize: 8 * baseFontSize

            onClicked: gameBoard.acceptMove(false)
        }

        Button {
            anchors {
                right: parent.right
                bottom: parent.bottom
                rightMargin: 4 * baseFontSize
                bottomMargin: 2 * baseFontSize
            }
            visible: gameEngine.stage === GameEngine.ConnectDotsStage
                     && !gameBoard.hasPendingMoves

            text: qsTr("End Turn")
            font.pixelSize: 10 * baseFontSize

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
                    text: qsTr("Resume")
                    font {
                        family: handwritingFont.name
                        pixelSize: 12 * baseFontSize
                    }

                    onClicked: overlayMenu.state = "hidden"
                }

                Button {
                    text: qsTr("End Game")
                    font {
                        family: handwritingFont.name
                        pixelSize: 12 * baseFontSize
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

                    text: qsTr("Are you sure you want to exit?")
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

                    text: qsTr("Yes")
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

                Button {
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                        leftMargin: 10
                        rightMargin: 10
                    }

                    text: qsTr("No")
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
