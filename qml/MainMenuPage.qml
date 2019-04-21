import QtQuick 2.9

CoverPage {
    id: page

    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
    }

    Column {
        anchors.centerIn: parent

        spacing: page.height * 0.125

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
                id:logoText

                anchors.horizontalCenter: parent.horizontalCenter

                source: "qrc:/images/logo_text.png"
                sourceSize.width: icon.width * 1.1
                fillMode: Image.PreserveAspectFit
            }
        }

        Flow {
            id: menu

            anchors.horizontalCenter: parent.horizontalCenter

            flow: page.width > page.height ? Flow.LeftToRight : Flow.TopToBottom
            spacing: flow === Flow.LeftToRight ? 20 * baseFontSize : 4 * baseFontSize

            Button {
                id: playButton

                text: qsTr("Play")
                font {
                    family: handwritingFont.name
                    pixelSize: 17 * baseFontSize
                }

                onClicked: pageRequested("newGamePage")
            }

            Button {
                id: howToPlayButton

                text: qsTr("How To Play")
                font {
                    family: handwritingFont.name
                    pixelSize: 15 * baseFontSize
                }

                onClicked: pageRequested("howToPlayPage")
            }

            Button {
                id: exitButton

                text: qsTr("Exit")
                font {
                    family: handwritingFont.name
                    pixelSize: 15 * baseFontSize
                }

                onClicked: Qt.quit()
            }
        }
    }
}
