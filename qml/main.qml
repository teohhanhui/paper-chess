// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: base

    GamePage {
        id: gamePage

        width: parent.width
        height: parent.height

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
        id:howToPlayPage

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
        id:mainMenuPage

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
