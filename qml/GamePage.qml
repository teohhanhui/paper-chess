// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import PaperChess 1.0

Page {
    id: page

    FontLoader {
        id: regularFont

        source: "qrc:/fonts/OpenSans-Regular.ttf"
    }

    FontLoader {
        id: boldFont

        source: "qrc:/fonts/Nunito-Bold.ttf"
    }

    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
    }

    Connections {
        target: gameEngine

        onGameStarted: gameTracker.state = "placeDotWaiting"

        onTurnEnded: gameTracker.state = "placeDotWaiting"

        onTurnsLeftChanged: {
            if (gameEngine.turnsLeft === 0) {
                gameTracker.state = "ended"
            }
        }
    }

    Item {
        id: gameTracker

        states: [
            State {
                name: "placeDotWaiting"
                PropertyChanges {
                    target: stageText
                    text: qsTr("Place Dot")
                }
                PropertyChanges {
                    target: confirmActionButton
                    visible: false
                    text: qsTr("Place")
                }
                PropertyChanges {
                    target: cancelActionButton
                    visible: false
                }
            },
            State {
                name: "placeDotPending"
                extend: "placeDotWaiting"
                PropertyChanges {
                    target: confirmActionButton
                    visible: true
                }
            },
            State {
                name: "connectDotsWaiting"
                extend: "placeDotPending"
                PropertyChanges {
                    target: stageText
                    text: qsTr("Connect Dots")
                }
                PropertyChanges {
                    target: confirmActionButton
                    text: qsTr("End Turn")
                }
            },
            State {
                name: "connectDotsPending"
                extend: "connectDotsWaiting"
                PropertyChanges {
                    target: confirmActionButton
                    text: qsTr("Connect")
                }
                PropertyChanges {
                    target: cancelActionButton
                    visible: true
                }
            },
            State {
                name: "ended"
                PropertyChanges {
                    target: stageText
                    text: qsTr("Game Ended")
                }
                PropertyChanges {
                    target: confirmActionButton
                    visible: false
                }
                PropertyChanges {
                    target: cancelActionButton
                    visible: false
                }
            }
        ]
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

            onHasPendingMovesChanged: {
                var stage = gameEngine.stage
                var pending = gameBoard.hasPendingMoves

                if (stage === GameEngine.PlaceDotStage) {
                    if (pending) {
                        gameTracker.state = "placeDotPending"
                    }
                    else {
                        gameTracker.state = "placeDotWaiting"
                    }
                }
                else if (stage === GameEngine.ConnectDotsStage) {
                    if (pending) {
                        gameTracker.state = "connectDotsPending"
                    }
                    else {
                        gameTracker.state = "connectDotsWaiting"
                    }
                }
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

            onClicked: {
                touchy.touchedPoint = Qt.point(mouseX, mouseY)
                gameBoard.markPosition(touchy.touchedPoint)
            }

            onDoubleClicked: {
                var newScale

                if (flicky.contentWidth > flicky.width) {
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
            GradientStop { position: 1.0; color: "#d5a95e" }
            GradientStop { position: 0.0; color: "#b78530" }
        }

        PlayerIndicator {
            id: player1Indicator

            width: 32 * baseFontSize;
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            state: gameEngine.currentPlayer == 0 ? "active" : "inactive"

            playerName: gameEngine.playerNames[0]
            playerMarkerSource: "qrc:/images/dot.svg"
            font {
                family: regularFont.name
                pixelSize: 6 * baseFontSize
            }
            activeColor: stageBar.color
        }

        Rectangle {
           id: dotsRemainingBar

           height: parent.height * 0.5
           anchors {
               left: player1Indicator.right
               right: player2Indicator.left
           }

           color: "transparent"

           Text {
               id: dotsRemainingText

               anchors.centerIn: parent

               text: qsTr("Dots: ") + gameEngine.turnsLeft
               font {
                   family: regularFont.name
                   pixelSize: 6 * baseFontSize
               }
           }

           states: [
               State {
                   name: "placedDot"
                   when: gameEngine.stage === GameEngine.ConnectDotsStage
                   PropertyChanges {
                       target: dotsRemainingText
                       text: qsTr("Dots: ") + (gameEngine.turnsLeft - 1)
                   }
               }
           ]
        }

        Rectangle {
            id: stageBar

            anchors {
                left: player1Indicator.right
                right: player2Indicator.left
                top: dotsRemainingBar.bottom
                bottom: parent.bottom
            }

            color: Qt.rgba(255, 128, 0, 0.25)

            Text {
                id: stageText

                anchors.centerIn: parent

                font {
                    family: boldFont.name
                    pixelSize: 8 * baseFontSize
                }
            }
        }

        PlayerIndicator {
            id: player2Indicator

            width: 32 * baseFontSize;
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            state: gameEngine.currentPlayer == 1 ? "active" : "inactive"

            playerName: gameEngine.playerNames[1]
            playerMarkerSource: "qrc:/images/cross.svg"
            font {
                family: regularFont.name
                pixelSize: 6 * baseFontSize
            }
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
            id: menuButton

            anchors {
                left: parent.left
                bottom: parent.bottom
                leftMargin: 4 * baseFontSize
                bottomMargin: 2 * baseFontSize
            }

            text: qsTr("Menu")
            font {
                family: regularFont.name
                pixelSize: 10 * baseFontSize
            }

            onClicked: overlayMenu.state = "shown"
        }

        Rectangle {
            id: actionsBar

            anchors {
                left: menuButton.right
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            color: "transparent"

            Button {
                id: cancelActionButton

                anchors {
                    right: confirmActionButton.left
                    bottom: parent.bottom
                    rightMargin: 10 * baseFontSize
                    bottomMargin: 3 * baseFontSize
                }

                text: qsTr("Cancel")
                font {
                    family: regularFont.name
                    pixelSize: 8 * baseFontSize
                }

                onClicked: gameBoard.acceptMove(false)
            }

            Button {
                id: confirmActionButton

                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    rightMargin: 4 * baseFontSize
                    bottomMargin: 2 * baseFontSize
                }

                text: "End Turn"
                font {
                    family: boldFont.name
                    pixelSize: 10 * baseFontSize
                }

                onClicked: {
                    if (gameTracker.state === "connectDotsWaiting") {
                        gameEngine.endTurn()
                    }
                    else if (gameTracker.state !== "endStage") {
                        gameBoard.acceptMove(true)
                    }
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
