// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

CoverPage {
    id: page

    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
    }

    Column {
        id: title

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: 5 * baseFontSize
        }

        Image {
            anchors.horizontalCenter: parent.horizontalCenter

            source: "qrc:/images/scoreboard_text.png"
            sourceSize.width: parent.width
            fillMode: Image.PreserveAspectFit
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: title.bottom
            leftMargin: 5 * baseFontSize
            rightMargin: 5 * baseFontSize
            topMargin: parent.height * 0.15
        }

        spacing: parent.height * 0.025

        Text {
            anchors {
                left: parent.left
                right: parent.right
            }

            text: gameEngine.playerNames[0] + "\n" + gameEngine.playerScores[0]
            font {
               family: handwritingFont.name
               pixelSize: 12 * baseFontSize
            }
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            anchors {
                left: parent.left
                right: parent.right
            }

            text: gameEngine.playerNames[1] + "\n" + gameEngine.playerScores[1]
            font {
               family: handwritingFont.name
               pixelSize: 12 * baseFontSize
            }
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Button {
        anchors {
            left: parent.left
            bottom: parent.bottom
            leftMargin: 5 * baseFontSize
            bottomMargin: 2 * baseFontSize
        }

        text: qsTr("Main Menu")
        font {
            family: handwritingFont.name
            pixelSize: 12 * baseFontSize
        }

        onClicked: pageRequested("mainMenuPage")
    }
}
