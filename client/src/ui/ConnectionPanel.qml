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
                text: "Enter server details to connect / Введите данные сервера, чтобы подключиться"
                wrapMode: Text.Wrap
                color: "#6a564d"
                Layout.fillWidth: true
            }
        }

        RowLayout {
            spacing: 8

            Rectangle {
                width: 10
                height: 10
                radius: 5
                color: Client.connected ? "#2d8a45" : "#c45454"
                Layout.alignment: Qt.AlignVCenter
            }

            Label {
                text: Client.connected ? "Online" : "Offline"
                font.bold: true
                color: "#2f241f"
                Layout.alignment: Qt.AlignVCenter
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
            text: Client.connected ? "Disconnect" : "Connect"
            Layout.fillWidth: true
            implicitHeight: 44
            onClicked: {
                if (Client.connected)
                    Client.disconnect()
                else
                    Client.connectTo(hostField.text, parseInt(portField.text))
            }
        }
    }
}
