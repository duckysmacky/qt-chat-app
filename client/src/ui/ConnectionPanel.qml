import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Rectangle {
    id: root
    color: "#f7efe5"
    radius: 16
    border.color: "#ddcdb9"
    border.width: 1

    implicitWidth: 260

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 14

        ColumnLayout {
            spacing: 4

            Label {
                text: "Connection"
                font.pixelSize: 22
                font.bold: true
                color: "#2f241f"
            }

            Label {
                text: "Enter server details to connect"
                wrapMode: Text.Wrap
                color: "#6a564d"
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            spacing: 10
            Layout.fillWidth: true

            Label {
                text: "Host"
                color: "#4f4038"
            }

            TextField {
                id: hostField
                text: "127.0.0.1"
                placeholderText: "127.0.0.1"
                Layout.fillWidth: true
                enabled: !Client.connected
            }

            Label {
                text: "Port"
                color: "#4f4038"
            }

            TextField {
                id: portField
                text: "8080"
                placeholderText: "8080"
                inputMethodHints: Qt.ImhDigitsOnly
                Layout.fillWidth: true
                enabled: !Client.connected
            }

            Label {
                text: Client.statusText
                color: "#6a564d"
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Button {
            text: "Connect"
            Layout.fillWidth: true
            implicitHeight: 44
            enabled: !Client.connected
            onClicked: {
                Client.connectTo(hostField.text, parseInt(portField.text))
            }
        }
    }
}
