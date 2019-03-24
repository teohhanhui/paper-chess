import QtQuick 2.9

CoverPage {
    id: page

    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
    }

    Column {
        id: centerColumn

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: logoText.height * 1.5
        }

        spacing: logoText.height

        Column {
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: logoText.height * 0.5

            Image {
                id: icon

                anchors.horizontalCenter: parent.horizontalCenter

                source: "qrc:/images/logo_icon.svg"
                sourceSize.height: page.height / 3
            }

            Image {
                id: logoText

                anchors.horizontalCenter: parent.horizontalCenter

                source: "qrc:/images/logo_text.png"
                sourceSize.width: icon.width * 1.1
                fillMode: Image.PreserveAspectFit
            }
        }

        Column {
            id: nameFields

            spacing: player1TextField.height * 0.4

            CoverTextField {
                id: player1TextField

                labelText: qsTr("Name")
                defaultText: qsTr("Player 1")
                maximumLength: 18
                labelFontSize: 8 * baseFontSize
                textFontSize: 8 * baseFontSize
            }

            CoverTextField {
                id: player2TextField

                labelText: qsTr("Name")
                defaultText: qsTr("Player 2")
                maximumLength: 18
                labelFontSize: 8 * baseFontSize
                textFontSize: 8 * baseFontSize
            }
        }
    }

    Item {
        height: parent.height * 0.2
        anchors {
            left: parent.left
            right: parent.right
            top: centerColumn.bottom
        }

        Button {
            id: startButton

            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            text: qsTr("Start")
            font {
                family: handwritingFont.name
                pixelSize: 15 * baseFontSize
            }

            onClicked: {
                gameEngine.newGame(40, 25, 200)
                gameEngine.playerNames = [player1TextField.text, player2TextField.text]

                pageRequested("gamePage")
            }
        }
    }

    Button {
        id: backButton

        anchors {
            left: parent.left
            bottom: parent.bottom
            leftMargin: 5 * baseFontSize
        }

        text: qsTr("Back")
        font {
            family: handwritingFont.name
            pixelSize: 12 * baseFontSize
        }

        onClicked: pageRequested("mainMenuPage")
    }
}
