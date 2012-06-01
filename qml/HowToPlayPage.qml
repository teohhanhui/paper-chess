// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

CoverPage {
    id: page

    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
    }

    Item {
        anchors {
            fill: parent
            margins: 6 * baseFontSize
        }

        Text {
            id: rulesTitle

            text: "Game Rules"
            font {
                family: handwritingFont.name
                pixelSize: 12 * baseFontSize
            }
        }

        Text {
            id: rulesContent

            anchors {
                left: parent.left
                right: parent.right
                top: rulesTitle.bottom
            }

            text: "<ol>
                     <li>Place dots on the grid.</li>
                     <li>Join the dots to capture opponent dots.</li>
                     <li>The player who captures the most dots wins.</li>
                   </ol>"
            font {
                family: handwritingFont.name
                pixelSize: 10 * baseFontSize
            }
            lineHeight: 0.8
            wrapMode: Text.WordWrap
        }

        Text {
            id: controlsTitle

            anchors {
                top: rulesContent.bottom
                topMargin: 5 * baseFontSize
            }

            text: "Game Controls"
            font {
                family: handwritingFont.name
                pixelSize: 12 * baseFontSize
            }
        }

        Text {
            id: controlsContent

            anchors {
                left: parent.left
                right: parent.right
                top: controlsTitle.bottom
            }

            text: "<ul>
                     <li> Insert/connect dots: Single tap</li>
                     <li> Zoom: Double tap or pinch</li>
                  </ul>"
            font {
                family: handwritingFont.name
                pixelSize: 10 * baseFontSize
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
        }

        text: qsTr("Back")
        font {
            family: handwritingFont.name
            pixelSize: 12 * baseFontSize
        }

        onClicked: pageRequested("mainMenuPage")
    }
}
