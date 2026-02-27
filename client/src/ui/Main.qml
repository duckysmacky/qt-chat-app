import QtQuick
import QtQuick.Controls
import client 1.0

Window {
    width: 640
    height: 320
    visible: true
    title: "Client"

    Client {
        id: connection
    }

    Row {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Column {
            spacing: 12
            width: 200

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

        Rectangle {
            color: "#f4f4f4"
            radius: 6
            border.color: "#d0d0d0"
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width - 200 - 16

            ListView {
                anchors.fill: parent
                anchors.margins: 8
                model: connection.messages
                clip: true
                delegate: Text {
                    text: modelData
                    wrapMode: Text.Wrap
                }
            }
        }
    }
}
