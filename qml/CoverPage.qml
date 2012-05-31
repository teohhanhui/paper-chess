// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
//page properties - setting the background and size
Page {
    Image {
        id: background

        anchors.fill: parent

        source: "images/cover.jpg"
        sourceSize.width: width
        fillMode: Image.PreserveAspectCrop
    }
}
