import QtQuick 2.9
import QtQuick.Window 2.2

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Paper Chess")

    Rectangle {
        id: base

        width: parent.width
        height: parent.height

        GamePage {
            id: gamePage

            width: parent.width
            height: parent.height
            state: "hidden"

            onPageRequested: {
                if (pageName === "scorePage") {
                    scorePage.state = "shown"
                }
            }
        }

        ScorePage {
            id: scorePage

            width: parent.width
            height: parent.height
            state: "hiddenRight"

            onPageRequested: {
                if (pageName === "mainMenuPage") {
                    mainMenuPage.state = "shown"
                    newGamePage.state = "shown"
                    scorePage.state = "hiddenRight"
                }
            }
        }

        NewGamePage {
            id: newGamePage

            width: parent.width
            height: parent.height
            state: "hidden"

            onPageRequested: {
                if (pageName === "gamePage") {
                    gamePage.state = "shown"
                    newGamePage.state = "hiddenLeft"
                }
                else if (pageName === "mainMenuPage") {
                    mainMenuPage.state = "shown"
                }
            }
        }

        HowToPlayPage {
            id: howToPlayPage

            width: parent.width
            height: parent.height
            state: "hidden"

            onPageRequested: {
                if (pageName === "mainMenuPage") {
                    mainMenuPage.state = "shown"
                }
            }
        }

        MainMenuPage {
            id: mainMenuPage

            width: parent.width
            height: parent.height
            state: "shown"

            onPageRequested: {
                if (pageName === "newGamePage") {
                    howToPlayPage.state = "hidden"
                    newGamePage.state = "shown"
                    mainMenuPage.state = "hiddenLeft"
                }
                else if (pageName === "howToPlayPage") {
                    howToPlayPage.state = "shown"
                    mainMenuPage.state = "hiddenLeft"
                }
            }
        }

        SplashScreen {
            id: splash

            width: parent.width
            height: parent.height
        }
    }
}
