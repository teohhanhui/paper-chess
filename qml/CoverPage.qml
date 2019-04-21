import QtQuick 2.9

Page {
    Image {
        id: background

        anchors.fill: parent

        source: "qrc:/images/cover.jpg"
        sourceSize.width: width
        fillMode: Image.PreserveAspectCrop
    }
}
