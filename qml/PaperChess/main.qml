// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import CustomComponents 1.0

Rectangle {
    width: 360
    height: 360

    GridItem {
        anchors.fill: parent
        anchors.margins: 10
        rows: 20
        columns: 20
        stroke.color: "lightblue"
        stroke.width: 1
    }
}
