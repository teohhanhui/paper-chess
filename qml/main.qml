// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: base

    GamePage {
        id: gamePage

        width: parent.width
        height: parent.height
        state: "hidden"

        onPageRequested: {
            if (pageName === "mainMenuPage") {
                mainMenuPage.state = "shown"
                gamePage.state = "hidden"
                newGamePage.state = "hidden"
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
                gamePage.player1Name = newGamePage.player1Name
                gamePage.player2Name = newGamePage.player2Name
            }
            else if (pageName === "mainMenuPage") {
                mainMenuPage.state="shown"
                newGamePage.state = "hidden"
            }
        }
    }

    HowToPlayPage {
        id:howToPlayPage

        width: parent.width
        height: parent.height
        state: "hidden"

        onPageRequested: {
            if(pageName === "mainMenuPage") {
                mainMenuPage.state = "shown"
                howToPlayPage.state = "hidden"
            }
        }
    }

    MainMenuPage {
        id:mainMenuPage

        width: parent.width
        height: parent.height

        state: "shown"

        onPageRequested: {
            if(pageName === "newGamePage") {
                newGamePage.state = "shown"
                mainMenuPage.state = "hiddenLeft"
            }
            else if(pageName === "howToPlayPage") {
                howToPlayPage.state = "shown"
                mainMenuPage.state = "hiddenLeft"
            }
        }
    }

    ScorePage {
        id: scorePage

        width: parent.width
        height: parent.height
        visible: false
    }

    SplashScreen {
        id: splash

        width: parent.width
        height: parent.height
    }
}
