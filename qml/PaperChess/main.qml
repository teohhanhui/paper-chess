// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: base

    GamePage {
        id: gamePage

        width: parent.width
        height: parent.height
        visible: false

        onPageRequested: {
            if (pageName === "newGamePage") {
                newGamePage.state = "shown"
            }
        }
    }

    NewGamePage {
        id: newGamePage

        width: parent.width
        height: parent.height
        visible: false

        onPageRequested: {
            if (pageName === "gamePage") {
                gamePage.visible = true
                newGamePage.state = "hiddenLeft"
            }
            else if (pageName ==="mainMenuPage") {
                mainMenuPage.state="shown"
                newGamePage.state = "hiddenRight"
            }
        }
    }

    HowToPlayPage {
        id:howToPlayPage

        width: parent.width
        height: parent.height
        visible: false

        onPageRequested: {
            if(pageName === "mainMenuPage") {
                mainMenuPage.state = "shown"
                howToPlayPage.state = "hiddenRight"
            }
        }
    }

    MainMenuPage {
        id:mainMenuPage

        width: parent.width
        height: parent.height

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
