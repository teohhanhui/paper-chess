import QtQuick 2.9

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

        source: "qrc:/fonts/CantataOne-Regular.ttf"
    }

    FontLoader {
        id: handwritingFont

        source: "qrc:/fonts/CoveredByYourGrace.ttf"
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

            source: "qrc:/images/dot_border.svg"
            sourceSize.height: height
            fillMode: Image.TileHorizontally
        }
    }
}
