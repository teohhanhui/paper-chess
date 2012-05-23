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

            Button {
                id: playButton

                color: "transparent"
                text: qsTr("Play")
                font {
                    family: handwritingFont.name
                    pixelSize: 15 * baseFontSize
                }
                labelColor: "black"
                labelHoverColor: "#454343" // graphite

                onClicked: pageRequested("newGamePage")
            }

            Button {
                id: howToPlayButton

                color: "transparent"
                text: qsTr("How To Play")
                font {
                    family: handwritingFont.name
                    pixelSize: 13 * baseFontSize
                }
                labelColor: "black"
                labelHoverColor: "#454343" // graphite

                onClicked: pageRequested("howToPlayPage")
            }

            Button {
                id: exitButton

                color: "transparent"
                text: qsTr("Exit")
                font {
                    family: handwritingFont.name
                    pixelSize: 13 * baseFontSize
                }
                labelColor: "black"
                labelHoverColor: "#454343" // graphite

                onClicked: Qt.quit()
            }
        }
    }
}
