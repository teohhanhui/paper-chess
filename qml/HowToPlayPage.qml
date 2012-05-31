// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
//tutorial page
CoverPage {
    id: page

    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
    }

    Item {
        anchors {
            fill: parent
            margins: 4 * baseFontSize
        }

        Text {
            id: title

            color: "black"
            text: "How to Play"
            font {
                family: handwritingFont.name
                pixelSize: 16 * baseFontSize
            }
        }

        Text {
            id: content

            width: parent.width
            anchors {
                top: title.bottom
            }

            color: "black"
            text: "<ol>
                     <li>Place dots on the grid.</li>
                     <li>Join the dots to capture opponent dots.</li>
                     <li>The player who captures the most dots wins.</li>
                   </ol>"
            font {
                family: handwritingFont.name
                pixelSize: 12 * baseFontSize
            }
            lineHeight: 0.8
            wrapMode: Text.WordWrap
        }
    }

    Button {
        anchors {
            left: parent.left
            bottom: parent.bottom
            leftMargin: 5 * baseFontSize
            bottomMargin: 2 * baseFontSize
        }

        text: qsTr("Back")
        font {
            family: handwritingFont.name
            pixelSize: 12 * baseFontSize
        }

        onClicked: pageRequested("mainMenuPage")
    }
}
