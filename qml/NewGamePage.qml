// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

CoverPage {
    id: page

    property alias player1Name: player1TextField.text
    property alias player2Name: player2TextField.text

    FontLoader {
        id: handwritingFont

        source: "fonts/CoveredByYourGrace.ttf"
    }

    Column {
        anchors.centerIn: parent

        Column {
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: logoText.height / 2

            Image {
                id: icon

                anchors.horizontalCenter: parent.horizontalCenter

                source: "images/logo_icon.svg"
                sourceSize.height: page.height / 3
            }

            Image {
                id: logoText

                anchors.horizontalCenter: parent.horizontalCenter

                source: "images/logo_text.png"
                sourceSize.width: icon.width * 1.1
                fillMode: Image.PreserveAspectFit
            }
        }

        Item {
            width: parent.width
            height: logoText.height * 2
        }

        Flow {
            id: newGameMenu

            anchors.horizontalCenter: parent.horizontalCenter

            flow: page.width > page.height ? Flow.LeftToRight : Flow.TopToBottom
            spacing: flow === Flow.LeftToRight ? 10 * baseFontSize : 2 * baseFontSize

            Column {
                spacing: player1TextField.height * 0.4

                CoverTextField {
                    id: player1TextField

                    labelText: "Name"
                    defaultText: "Player 1"
                    maximumLength: 18
                    labelFontSize: 8 * baseFontSize
                    textFontSize: 8 * baseFontSize
                }

                CoverTextField {
                    id: player2TextField

                    labelText: "Name"
                    defaultText: "Player 2"
                    maximumLength: 18
                    labelFontSize: 8 * baseFontSize
                    textFontSize: 8 * baseFontSize
                }
            }

            Button {
                id: startButton

                text: qsTr("Start")
                font {
                    family: handwritingFont.name
                    pixelSize: 13 * baseFontSize
                }

                onClicked: pageRequested("gamePage")
            }

            Button {
                id: backButton

                text: qsTr("Back")
                font {
                    family: handwritingFont.name
                    pixelSize: 12 * baseFontSize
                }

                onClicked: pageRequested("mainMenuPage")
            }
        }
    }
}
