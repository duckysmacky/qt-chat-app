import QtQuick
import QtQuick.Controls
import client 1.0

Window {
    width: 400
    height: 240
    visible: true
    title: "Client"

    Client {
        id: connection
    }

    Column {
        anchors.centerIn: parent
        spacing: 12
        width: parent.width * 0.8

        TextField {
            id: hostField
            placeholderText: "Host"
            text: "127.0.0.1"
        }

        TextField {
            id: portField
            placeholderText: "Port"
            text: "8080"
            inputMethodHints: Qt.ImhDigitsOnly
        }

        Button {
            text: "Connect"
            onClicked: connection.connectTo(hostField.text, parseInt(portField.text))
        }

        Text {
            text: connection.statusText
            color: connection.statusText === "Connected" ? "green" : "red"
            wrapMode: Text.Wrap
        }
    }
}
