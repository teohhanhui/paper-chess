// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

CoverPage {
    id: page

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
                id:logoText

                anchors.horizontalCenter: parent.horizontalCenter

                source: "images/logo_text.png"
                sourceSize.width: icon.width * 1.1
                fillMode: Image.PreserveAspectFit
            }
        }

        Item {
            width: parent.width
            height: page.height / 8
        }

        Flow {
            id: menu

            anchors.horizontalCenter: parent.horizontalCenter

            flow: page.width > page.height ? Flow.LeftToRight : Flow.TopToBottom
            spacing: flow === Flow.LeftToRight ? 20 * baseFontSize : 4 * baseFontSize

            CoverButton {
                id: playButton

                text: qsTr("Play")
                font.pixelSize: 15 * baseFontSize

                onClicked: pageRequested("newGamePage")
            }

            CoverButton {
                id: howToPlayButton

                text: qsTr("How To Play")
                font.pixelSize: 13 * baseFontSize

                onClicked: pageRequested("howToPlayPage")
            }

            CoverButton {
                id: exitButton

                text: qsTr("Exit")
                font.pixelSize: 13 * baseFontSize

                onClicked: Qt.quit()
            }
        }
    }
}