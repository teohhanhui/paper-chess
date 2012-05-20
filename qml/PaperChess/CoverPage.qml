// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    Image {
        id: background

        anchors.fill: parent

        source: "cover_texture.png"
        fillMode: Image.Tile
    }
}
