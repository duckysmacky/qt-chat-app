import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import ChatApp 1.0

Window {
    width: 640
    height: 320
    visible: true
    title: "Chat app"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        ColumnLayout {
            spacing: 12
            Layout.preferredWidth: 200

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
                text: Client.connected ? "Disconnect" : "Connect"
                onClicked: {
                    if (Client.connected)
                        Client.disconnect()
                    else
                        Client.connectTo(hostField.text, parseInt(portField.text))
                }
            }

            Text {
                text: Client.statusText
                color: Client.connected ? "green" : "red"
                wrapMode: Text.Wrap
            }
        }

        ChatBox {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 160
        }
    }
}
