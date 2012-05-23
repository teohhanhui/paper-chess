// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Flow {
    property string labelText
    property string defaultText
    property alias maximumLength: input.maximumLength
    property alias labelFontSize: label.font.pixelSize
    property alias textFontSize: input.font.pixelSize
    property alias text: input.text

    spacing: label.height * 0.5

    FontLoader {
        id: printFont

        source: "fonts/CantataOne-Regular.ttf"
    }

    FontLoader {
        id: handwritingFont

        source: "fonts/CoveredByYourGrace.ttf"
    }

    Text {
        id: label

        color: "black"
        text: labelText + " :"
        font.family: printFont.name
    }

    TextInput {
        id: input

        width: label.width * 2
        height: label.height

        text: defaultText
        font.family: handwritingFont.name

        Image {
            height: input.height * 0.1
            anchors {
                left: input.left
                right: input.right
                bottom: input.bottom
            }

            source: "images/dot_border.svg"
            sourceSize.height: height
            fillMode: Image.TileHorizontally
        }
    }
}
