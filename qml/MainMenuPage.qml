// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
//main menu page
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
            //logo image
            Image {
                id: icon

                anchors.horizontalCenter: parent.horizontalCenter

                source: "images/logo_icon.svg"
                sourceSize.height: page.height / 3
            }
            //logo text
            Image {
                id:logoText

                anchors.horizontalCenter: parent.horizontalCenter

                source: "images/logo_text.png"
                sourceSize.width: icon.width * 1.1
                fillMode: Image.PreserveAspectFit
            }
        }
        //for spacing
        Item {
            width: parent.width
            height: page.height / 8
        }
        //layout of menu item button
        Flow {
            id: menu

            anchors.horizontalCenter: parent.horizontalCenter
            //set layout position for potrait mode and landscape mode
            flow: page.width > page.height ? Flow.LeftToRight : Flow.TopToBottom
            spacing: flow === Flow.LeftToRight ? 20 * baseFontSize : 4 * baseFontSize

            Button {
                id: playButton

                text: qsTr("Play")
                font {
                    family: handwritingFont.name
                    pixelSize: 15 * baseFontSize
                }

                onClicked: pageRequested("newGamePage")
            }

            Button {
                id: howToPlayButton

                text: qsTr("How To Play")
                font {
                    family: handwritingFont.name
                    pixelSize: 13 * baseFontSize
                }

                onClicked: pageRequested("howToPlayPage")
            }

            Button {
                id: exitButton

                text: qsTr("Exit")
                font {
                    family: handwritingFont.name
                    pixelSize: 13 * baseFontSize
                }

                onClicked: Qt.quit()
            }
        }
    }
}
