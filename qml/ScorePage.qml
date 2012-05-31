// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

CoverPage {
    id: page

    property string player1Name
    property string player2Name

    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
    }

    Item {
        id: title

        height: 25 * baseFontSize
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        Image {
            anchors {
                fill: parent
                margins: 5 * baseFontSize
            }

            source: "qrc:/images/scoreboard_text.png"
            sourceSize.height: height
            fillMode: Image.PreserveAspectFit
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: title.bottom
            leftMargin: 5 * baseFontSize
            topMargin: 15 * baseFontSize
        }

        spacing: 10 * baseFontSize

        Text {
            text: player1Name + " score here"
            font {
               family: handwritingFont.name
               pixelSize: 12 * baseFontSize
            }
        }

        Text {
            text: player2Name + " score here"
            font {
               family: handwritingFont.name
               pixelSize: 12 * baseFontSize
            }
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
